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
        virtual size_t _write(uint8_t *data, uint8_t data_size) = 0;

        /**
         *
         * @param package
         * @param package_size
         * @return
         */
        virtual size_t _read(uint8_t *package, uint8_t package_size) = 0;

    public:
        err_t startup_error = err_t::NOT_STARTED;

        /**
         * @brief  directly transmit a package on the link.
         * @param  package: package to be send
         * @param  package_size: size of the package
         * @return
         */
        size_t transmit(uint8_t *package, uint8_t package_size);

        /**
         * Read data from the receive buffer.
         * @param buffer: write received data to buffer
         * @param buffer_size: amounts of bytes to read
         * @return amount of bytes received.
         */
        size_t receive(uint8_t *buffer, uint8_t buffer_size);

        /**
         * Get speed of link (bits per second)
         * @return unsigned int speed in Bytes per second.
         */
        virtual unsigned int get_speed() = 0;

        /**
         * Does the link have an active connection with a client?
         * @return true/false
         */
        bool is_connected();

    };

}
#endif //ARUNA_COMMLINK_H
