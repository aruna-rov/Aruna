//
// Created by noeel on 14-10-20.
//

#include "aruna/driver/port/ESP32/ESP32_I2C_master.h"
#include "driver/i2c.h"

using namespace aruna;
using namespace aruna::driver;

ESP32_I2C_master::ESP32_I2C_master(i2c_port_t i2c_num, const i2c_config_t* i2c_conf): i2c_num(i2c_num), i2c_conf(i2c_conf) {
//    start I2C driver
    ESP_ERROR_CHECK(i2c_param_config(i2c_num, i2c_conf));
    ESP_ERROR_CHECK(i2c_driver_install(i2c_num, I2C_MODE_MASTER, 0, 0, ESP_INTR_FLAG_IRAM));
    i2c_cmd_handler = i2c_cmd_link_create();
}

ESP32_I2C_master::~ESP32_I2C_master() {
    ESP_ERROR_CHECK(i2c_driver_delete(i2c_num));
    i2c_cmd_link_delete(i2c_cmd_handler);
}
err_t ESP32_I2C_master::_write(uint8_t address, uint8_t reg, uint8_t* data, size_t data_size) {
    esp_err_t esp_err;
    ESP_ERROR_CHECK(i2c_master_start(i2c_cmd_handler));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (address << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (uint8_t) reg, I2C_MASTER_ACK));
    ESP_ERROR_CHECK(i2c_master_write(i2c_cmd_handler, data, data_size, I2C_MASTER_ACK));
    ESP_ERROR_CHECK(i2c_master_stop(i2c_cmd_handler));
    esp_err = i2c_master_cmd_begin(i2c_num, i2c_cmd_handler, 100 / portTICK_PERIOD_MS );
    switch (esp_err) {
        case ESP_OK:
            return err_t::OK;
        case ESP_ERR_INVALID_ARG:
            return err_t::INVALID_PARAMETERS;
        case ESP_FAIL:
            return err_t::NO_RESPONSE;
        case ESP_ERR_INVALID_STATE:
            return err_t::HARDWARE_FAILURE;
        default:
            return err_t::UNDEFINED;
    }
}

err_t ESP32_I2C_master::_read(uint8_t address, uint8_t reg, uint8_t* buffer, size_t buffer_size) {
    esp_err_t esp_err;
    ESP_ERROR_CHECK(i2c_master_start(i2c_cmd_handler));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (address << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (uint8_t) reg, I2C_MASTER_ACK));
    ESP_ERROR_CHECK(i2c_master_start(i2c_cmd_handler));
    ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (address << 1) | I2C_MASTER_READ, true));
    ESP_ERROR_CHECK(i2c_master_read(i2c_cmd_handler, buffer, buffer_size, I2C_MASTER_ACK));
    ESP_ERROR_CHECK(i2c_master_stop(i2c_cmd_handler));
    esp_err = i2c_master_cmd_begin(i2c_num, i2c_cmd_handler, 100 / portTICK_PERIOD_MS );
    switch (esp_err) {
        case ESP_OK:
            return err_t::OK;
        case ESP_ERR_INVALID_ARG:
            return err_t::INVALID_PARAMETERS;
        case ESP_FAIL:
            return err_t::NO_RESPONSE;
        case ESP_ERR_INVALID_STATE:
            return err_t::HARDWARE_FAILURE;
        default:
            return err_t::UNDEFINED;
    }
}