//
// Created by noeel on 14-1-19.
//

#ifndef ARUNA_COMMLINK_H
#define ARUNA_COMMLINK_H

#include "aruna/arunaTypes.h"
#include <aruna/comm/commTypes.h>

namespace aruna::comm {
    class Link {
    private:
        /**
         * write data to the link/bus.
         * @param data: bytes to write
         * @param data_size: length of data
         * @return amount of bytes written.
         */
        virtual size_t _write(uint8_t *data, size_t data_size) = 0;

        /**
         * Read bytes and out them in the buffer
         * @param buffer: buffer to read
         * @param buffer_size: amount of bytes to read
         * @return amount of byte read
         */
        virtual size_t _read(uint8_t *buffer, size_t buffer_size) = 0;

    public:
        err_t startup_error = err_t::NOT_STARTED;

        /**
         * @brief  directly transmit a package on the link.
         * @param  package: package to be send
         * @param  package_size: size of the package
         * @return
         */
        size_t transmit(uint8_t *package, size_t package_size);

        /**
         * Read data from the receive buffer.
         * @param buffer: write received data to buffer
         * @param buffer_size: amounts of bytes to read
         * @return amount of bytes received.
         */
        size_t receive(uint8_t *buffer, size_t buffer_size);

//        TODO allow to receive and transmit comm_package directly.
//       TODO write IO abstraction driver for all read an write functionality so I2C_Master, UART an Link can all inherit from it.

        /**
         * Get speed of link (bits per second)
         * @return unsigned int speed in Bytes per second.
         */
        virtual uint32_t get_speed() = 0;

        /**
         * Does the link have an active connection with a client?
         * @return true/false
         */
        bool is_connected();

    };

}
#endif //ARUNA_COMMLINK_H
