//
// Created by noeel on 15-09-20.
//

#include "aruna/control/PCA9685.h"
#include "aruna/control/controlTypes.h"
#include "aruna/driver/PCA9685/PCA9685.h"
#include "driver/i2c.h"

using namespace aruna;
using namespace driver::PCA9685;
// TODO filename is the same as driver::PCA9685 (as is the header) maybe change it to avoid confusion?

control::PCA9685::PCA9685(control::axis_mask_t axis,
                          control::direction_t direction,
                          uint8_t led,
                          uint8_t i2c_address,
                          float min_duty_cycle_percentage,
                          float max_duty_cycle_percentage) :
        axis(axis),
        direction(direction),
        led(led),
        i2c_address(i2c_address),
        min_duty_cycle_percentage(min_duty_cycle_percentage),
        max_duty_cycle_percentage(max_duty_cycle_percentage) {

}

control::axis_mask_t control::PCA9685::get_axis() {
    return axis;
}

err_t control::PCA9685::set(control::axis_mask_t axisMask, uint16_t speed,
                            control::direction_t direction) {
    err_t ret_val = err_t::OK;
    if ((uint8_t) axisMask & (uint8_t) axis && (this->direction == direction_t::BOTH || this->direction == direction)) {
        uint16_t off_pos = (uint16_t) convert_range(speed, 4095 * (float) (max_duty_cycle_percentage/100), 4095 * (float) (min_duty_cycle_percentage/100));

        ret_val = driver::PCA9685::set_pwm(led, 0, off_pos, i2c_address);
    }
    return ret_val;
}

err_t control::PCA9685::start() {
    i2c_port_t I2C_port = I2C_NUM_0;
    i2c_cmd_handle_t i2c_cmd_handler;
    i2c_cmd_handler = i2c_cmd_link_create();
//    TODO this sequence should be in driver::PCA9685
//    TODO this sequence is run multiple times while only one time is required.
//    sleep
    ESP_ERROR_CHECK(i2c_master_start(i2c_cmd_handler));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (i2c_address << 1) | I2C_MASTER_WRITE, true));
    ESP_ERROR_CHECK(
            i2c_master_write_byte(i2c_cmd_handler, (uint8_t) driver::PCA9685::register_address_pointer::MODE1, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler,
                                          (uint8_t) RESTART::DEFAULT |
                                          (uint8_t) EXTCLK::DEFAULT |
                                          (uint8_t) AI::DEFAULT |
                                          (uint8_t) SLEEP::LOW_POWER |
                                          (uint8_t) SUB1::DEFAULT |
                                          (uint8_t) SUB2::DEFAULT |
                                          (uint8_t) SUB3::DEFAULT |
                                          (uint8_t) ALLCALL::DEFAULT, true));
//  set pre scale
    ESP_ERROR_CHECK(i2c_master_start(i2c_cmd_handler));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (i2c_address << 1) | I2C_MASTER_WRITE, true));
    ESP_ERROR_CHECK(
            i2c_master_write_byte(i2c_cmd_handler, (uint8_t) driver::PCA9685::register_address_pointer::PRE_SCALE, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (uint8_t) 0x7A, true));

//    write config
    ESP_ERROR_CHECK(i2c_master_start(i2c_cmd_handler));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (i2c_address << 1) | I2C_MASTER_WRITE, true));
    ESP_ERROR_CHECK(
            i2c_master_write_byte(i2c_cmd_handler, (uint8_t) driver::PCA9685::register_address_pointer::MODE1, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler,
                                          (uint8_t) RESTART::DISABLED |
                                          (uint8_t) EXTCLK::USE_INTERNAL_CLOCK |
                                          (uint8_t) AI::AUTO_INCREMENT_ENABLED |
                                          (uint8_t) SLEEP::NORMAL_MODE |
                                          (uint8_t) SUB1::DEFAULT |
                                          (uint8_t) SUB2::DEFAULT |
                                          (uint8_t) SUB3::DEFAULT |
                                          (uint8_t) ALLCALL::DEFAULT, true));
    ESP_ERROR_CHECK(i2c_master_stop(i2c_cmd_handler));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_port, i2c_cmd_handler, 1000 / portTICK_PERIOD_MS));

    i2c_cmd_link_delete(i2c_cmd_handler);
    set(this->get_axis(), 0, this->direction);
    return err_t::OK;
}
