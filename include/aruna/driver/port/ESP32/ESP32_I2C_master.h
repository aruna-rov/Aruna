//
// Created by noeel on 14-10-20.
//

#ifndef ARUNA_ESP32_I2C_MASTER_H
#define ARUNA_ESP32_I2C_MASTER_H

#include "aruna/driver/I2C_master.h"
#include "driver/i2c.h"

namespace aruna {
    namespace driver {
        class ESP32_I2C_master : public I2C_master {
        private:
            i2c_port_t i2c_num;
            const i2c_config_t *i2c_conf;
            i2c_cmd_handle_t i2c_cmd_handler;

            err_t _write(uint8_t address, uint8_t reg, uint8_t *data, size_t data_size) override;

            err_t _read(uint8_t address, uint8_t reg, uint8_t *buffer, size_t buffer_size) override;

        public:
            ESP32_I2C_master(i2c_port_t i2c_num, const i2c_config_t *i2c_conf);

            ~ESP32_I2C_master();
        };
    }
}


#endif //ARUNA_ESP32_I2C_MASTER_H
