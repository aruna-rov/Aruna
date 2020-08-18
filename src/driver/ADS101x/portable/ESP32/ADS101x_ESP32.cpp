//
// Created by noeel on 17-08-20.
//

#include "aruna/driver/ADS101x/ADS101x.h"
#include "stdint.h"
#include "driver/i2c.h"

namespace aruna {
    namespace driver {
        namespace ADS101x {
            int16_t readConversion(uint16_t config, uint8_t address) {
//                TODO make I2C port selectable
                i2c_port_t I2C_port = I2C_NUM_0;
                i2c_cmd_handle_t i2c_cmd_handler;
                i2c_cmd_handler = i2c_cmd_link_create();
                uint8_t data[2];
//                write config
                ESP_ERROR_CHECK(i2c_master_start(i2c_cmd_handler));
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (address << 1) | I2C_MASTER_WRITE, true));
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (uint8_t) register_address_pointer::CONFIG, true));
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, config >> 8, true));
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, config & 0xFF, true));
//                set Conversion register to read
                ESP_ERROR_CHECK(i2c_master_start(i2c_cmd_handler));
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (address << 1) | I2C_MASTER_WRITE, true));
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (uint8_t) register_address_pointer::CONVERSION, true));
//                read conversion register
                ESP_ERROR_CHECK(i2c_master_start(i2c_cmd_handler));
                ESP_ERROR_CHECK(i2c_master_write_byte(i2c_cmd_handler, (address << 1) | I2C_MASTER_READ, true));
                ESP_ERROR_CHECK(i2c_master_read(i2c_cmd_handler, &data[0], 2, I2C_MASTER_ACK));
                ESP_ERROR_CHECK(i2c_master_stop(i2c_cmd_handler));
                ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_port, i2c_cmd_handler, 1000 / portTICK_PERIOD_MS ));

                i2c_cmd_link_delete(i2c_cmd_handler);
                return (int16_t)( data[0] << 8 | (data[1] & 0xFF));
            }

//            TODO
            bool isConnected(uint8_t address);

        }
    }
}