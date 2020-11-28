//
// Created by noeel on 08-11-20.
//

#include "aruna/driver/port/ESP32/ESP32_RMT_Dshot.h"

using namespace aruna;
using namespace aruna::driver;
ESP32_RMT_Dshot::ESP32_RMT_Dshot(rmt_channel_t channel, gpio_num_t gpio_port) {
    const static uint32_t PERF_CLK_HZ = 80000000;
//                TODO get the clock speed dynamicly
    const static uint8_t clk_div = 1;

//    config driver
    driver_config.channel = channel;
    driver_config.gpio_num = gpio_port;
    driver_config.rmt_mode = RMT_MODE_TX;
    driver_config.clk_div = clk_div;
    driver_config.mem_block_num = 1;
    driver_config.tx_config.loop_en = false;
    driver_config.tx_config.carrier_en = false;
    driver_config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    driver_config.tx_config.idle_output_en = false;

//        calculate ticks
    const static float ticks_per_second_in_nanosecond = ((float) PERF_CLK_HZ / (float) clk_div) / 1000000000;

    T0H_ticks = ticks_per_second_in_nanosecond * T0H_ns;
    T1H_ticks = ticks_per_second_in_nanosecond * T1H_ns;
    T0L_ticks = ticks_per_second_in_nanosecond * T0L_ns;
    T1L_ticks = ticks_per_second_in_nanosecond * T1L_ns;

    err_t rval;
    esp_err_t eerr;
    ESP_ERROR_CHECK(rmt_config(&driver_config));
    eerr = rmt_driver_install(driver_config.channel, 0, 0);
//    TODO make generic esp_err_t to aruna::err_t function?
    switch (eerr) {
        case ESP_OK:
            rval = err_t::OK;
            break;
        case ESP_ERR_INVALID_ARG:
            rval = err_t::INVALID_PARAMETERS;
            break;
        case ESP_FAIL:
        default:
            rval = err_t::FAIL;
            break;
    }
    //    start thread
    int p_mut, p_cre = 0;
    p_mut = pthread_mutex_init(&dshot_frame_lock, NULL);
    p_cre = pthread_create(&update_handler, NULL, _update_task, this);
    if (p_mut || p_cre)
        rval = err_t::TASK_FAILED;
    startup_error = rval;
}

ESP32_RMT_Dshot::~ESP32_RMT_Dshot() {
    rmt_driver_uninstall(driver_config.channel);
    pthread_exit(&update_handler);
    pthread_mutex_destroy(&dshot_frame_lock);
}

void ESP32_RMT_Dshot::update_task() {

    while (true) {
        pthread_mutex_lock(&dshot_frame_lock);
        single_write(dshot_frame);
        pthread_mutex_unlock(&dshot_frame_lock);
        usleep(1);
    }

}

void *ESP32_RMT_Dshot::_update_task(void *_this) {
    static_cast<ESP32_RMT_Dshot *>(_this)->update_task();
    return 0;
}

err_t ESP32_RMT_Dshot::_write_frame_continuous(uint16_t dshot_frame) {
    pthread_mutex_lock(&dshot_frame_lock);
    this->dshot_frame = dshot_frame;
    pthread_mutex_unlock(&dshot_frame_lock);
    return err_t::OK;
}

err_t ESP32_RMT_Dshot::single_write(uint16_t dshot_frame) {
    const static size_t rmt_size = 17;
    static rmt_item32_t rmt_frame[rmt_size];

    for (uint8_t i = 0; i < rmt_size - 1; ++i) {
        uint16_t i_bit = (dshot_frame >> (rmt_size - 2 - i)) & 0b1;
        uint16_t on_time = i_bit ? T1H_ticks : T0H_ticks;
        uint16_t off_time = i_bit ? T1L_ticks : T0L_ticks;

        rmt_frame[i] = {{{on_time, 1, off_time, 0}}};
    }
//    clear at the end
    rmt_frame[rmt_size - 1] = {{{0, 1, 0, 0}}};

    ESP_ERROR_CHECK(rmt_write_items(driver_config.channel, rmt_frame, rmt_size, true));
//    TODO return error.
    return err_t::OK;
}
