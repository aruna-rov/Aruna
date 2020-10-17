//
// Created by noeel on 14-10-20.
//

#ifndef ARUNA_I2C_MASTER_H
#define ARUNA_I2C_MASTER_H

#include <aruna/arunaTypes.h>
#include "pthread.h"
namespace aruna {
    namespace driver {
        class I2C_master {
        private:
            pthread_mutex_t line_busy;
            pthread_mutexattr_t line_busy_attr;
            virtual err_t _write(uint8_t address, uint8_t reg, uint8_t* data, size_t data_size) = 0;
            virtual err_t _read(uint8_t address, uint8_t reg, uint8_t* buffer, size_t buffer_size) = 0;
        public:
            I2C_master();
            ~I2C_master();
            /**
             * Write data to the I²C bus
             * @param address: slave address
             * @param reg: register to write to
             * @param data: data array to be written.
             * @param data_size: number of bytes in data
             * @return
             *  - err_t::OK success
             *  - err_t::NO_RESPONSE slave doesn't ACK the transfer
             *  - err_t::INVALID_PARAMETERS parameter error
             *  - err_t::HARDWARE_FAILURE I2C driver malfunction
             */
            err_t write(uint8_t address, uint8_t reg, uint8_t* data, size_t data_size);

            /**
             * Read data from I²C slave
             * @param address: slave address
             * @param reg: register to write to
             * @param buffer: buffer to store read data
             * @param buffer_size: amount of bytes to read.
             * @return
             *  - err_t::OK success
             *  - err_t::NO_RESPONSE slave doesn't ACK the transfer
             *  - err_t::INVALID_PARAMETERS parameter error
             *  - err_t::HARDWARE_FAILURE I2C driver malfunction
             */
            err_t read(uint8_t address, uint8_t reg, uint8_t* buffer, size_t buffer_size);
        };
    }
}



#endif //ARUNA_I2C_MASTER_H
