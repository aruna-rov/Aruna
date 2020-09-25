//
// Created by noeel on 15-09-20.
//

#include "aruna/driver/PCA9685/PCA9685.h"
#include "driver/i2c.h"

namespace aruna {
    namespace driver {
        namespace PCA9685 {
//            TODO create I²C abstraction
            err_t set_pwm(uint8_t led, uint16_t on, uint16_t off, uint8_t address) {

                i2c_port_t I2C_port = I2C_NUM_0;
                i2c_cmd_handle_t i2c_cmd_handler;
                i2c_cmd_handler = i2c_cmd_link_create();
                ESP_ERROR_CHECK(i2c_master_start(i2c_cmd_handler));
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (address << 1) | I2C_MASTER_WRITE, true));
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (uint8_t) register_address_pointer::LED0_ON_L + 4 * led, true));
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, on, true));
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, on >> 8, true));
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, off, true));
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, off >> 8, true));
                ESP_ERROR_CHECK(i2c_master_stop(i2c_cmd_handler));
                ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_port, i2c_cmd_handler, 1000 / portTICK_PERIOD_MS ));

                i2c_cmd_link_delete(i2c_cmd_handler);
                return err_t::OK;
            }
        }
    }
}