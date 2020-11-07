//
// Created by noeel on 22-03-20.
//

#include "aruna/control/portable/esp32/Dshot.h"

using namespace aruna::control;


esp32::Dshot::Dshot(axis_mask_t axis, rmt_channel_t channel, gpio_num_t gpio_port) :
        Actuator(axis), axis(axis) {
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
//    TODO 300khz also works, make option to switch
    const static uint32_t speed_hz = 150000;
    const static uint32_t max_ns = (1.f / (float) speed_hz) * 1000000000;

    const static float T0H_percentage = 0.375;
    const static float T1H_percentage = 0.75;
    const static float T0L_percentage = 1 - T0H_percentage;
    const static float T1L_percentage = 1 - T1H_percentage;

    const static uint16_t T0H_ns = max_ns * T0H_percentage;
    const static uint16_t T1H_ns = max_ns * T1H_percentage;
    const static uint16_t T0L_ns = max_ns * T0L_percentage;
    const static uint16_t T1L_ns = max_ns * T1L_percentage;

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
//    arm ESC
    uint16_t start_value = 48;
    uint8_t step = 50;
    uint8_t amount_of_steps = 40;

//    up hill
    for (int i = 0; i < amount_of_steps; ++i) {
        bits_to_dshotFrame(create_dshotFrame((step * i) + start_value, 0), dshot_frame);
        ESP_ERROR_CHECK(rmt_write_items(driver_config.channel, dshot_frame, rmt_size, true));
        vTaskDelay(1);
    }
//    down hill
    for (int i = amount_of_steps - 1; i > -1; --i) {
        bits_to_dshotFrame(create_dshotFrame((step * i) + start_value, 0), dshot_frame);
        ESP_ERROR_CHECK(rmt_write_items(driver_config.channel, dshot_frame, rmt_size, true));
        vTaskDelay(1);
    }
//    stay low for a little while
    for (int i = 0; i < 150; ++i) {
        bits_to_dshotFrame(create_dshotFrame(start_value, 0), dshot_frame);
        ESP_ERROR_CHECK(rmt_write_items(driver_config.channel, dshot_frame, rmt_size, true));
        vTaskDelay(1);
    }

    //    start thread
    int p_mut = 0;
    int p_cre = 0;
    p_mut = pthread_mutex_init(&dshot_frame_lock, NULL);
    p_cre = pthread_create(&update_handler, NULL, __update_task, this);
    if (p_mut || p_cre)
        rval = err_t::TASK_FAILED;
    startup_error = rval;
}

esp32::Dshot::~Dshot() {
    rmt_driver_uninstall(driver_config.channel);
    pthread_exit(&update_handler);
    pthread_mutex_destroy(&dshot_frame_lock);
}

aruna::err_t esp32::Dshot::_set(axis_mask_t axisMask, int16_t speed) {
    const static uint16_t MAX_VALUE = 2047;
    const static uint16_t MIN_VALUE = 48;
    const static uint16_t MID_VALUE = (MAX_VALUE + MIN_VALUE) / 2;
    const static uint8_t telemetry = 0;

    uint16_t max, min = 0;
    if ((uint8_t) this->axis & (uint8_t) axis_mask_t::DIRECTION_BOTH) {
        max = speed < 0 ? MID_VALUE : MAX_VALUE;
        min = speed < 0 ? MIN_VALUE : MID_VALUE;
    } else {
        max = MAX_VALUE;
        min = MIN_VALUE;
    }
    uint16_t adjusted_speed = (uint16_t) convert_range(speed, max, min);
    adjusted_speed = speed ? adjusted_speed : MIN_VALUE;
    uint16_t bit_frame = create_dshotFrame(adjusted_speed, telemetry);

    pthread_mutex_lock(&dshot_frame_lock);
    bits_to_dshotFrame(bit_frame, dshot_frame);
    pthread_mutex_unlock(&dshot_frame_lock);
    return err_t::OK;
}


void esp32::Dshot::bits_to_dshotFrame(uint16_t bits, rmt_item32_t *frame_buffer) {

    for (uint8_t i = 0; i < rmt_size - 1; ++i) {
        uint16_t i_bit = (bits >> (rmt_size - 2 - i)) & 0b1;
        uint16_t on_time = i_bit ? T1H_ticks : T0H_ticks;
        uint16_t off_time = i_bit ? T1L_ticks : T0L_ticks;

        frame_buffer[i] = {{{on_time, 1, off_time, 0}}};
    }
//    clear at the end
    frame_buffer[rmt_size - 1] = {{{0, 1, 0, 0}}};
}

void esp32::Dshot::add_CRC(uint16_t &bits) {
    uint8_t csum = 0;
    uint16_t csum_data = bits >> 4;
    for (uint8_t i = 0; i < 3; i++) {
        csum ^= csum_data;
        csum_data >>= 4;
    }
    csum &= 0xf;
    bits |= csum;
}

void esp32::Dshot::update_task() {

    while (true) {
        pthread_mutex_lock(&dshot_frame_lock);
        ESP_ERROR_CHECK(rmt_write_items(driver_config.channel, dshot_frame, rmt_size, true))
        pthread_mutex_unlock(&dshot_frame_lock);
        vTaskDelay(1);
    }

}

void *esp32::Dshot::__update_task(void *_this) {
    static_cast<Dshot *>(_this)->update_task();
    return 0;
}

uint16_t esp32::Dshot::create_dshotFrame(uint16_t speed, uint8_t telemetry) {
    uint16_t bit_frame = 0;
    bit_frame = speed << 5;
    bit_frame |= telemetry << 4;
    add_CRC(bit_frame);
    return bit_frame;
}
