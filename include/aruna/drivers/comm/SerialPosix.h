//
// Created by noeel on 2-7-19.
//

#ifndef ARUNA_SERIALPOSIX_H
#define ARUNA_SERIALPOSIX_H

#include "aruna/arunaTypes.h"
#include <aruna/drivers/comm/CommDriver.h>
#include <aruna/log.h>

namespace aruna { namespace drivers { namespace comm {

            class SerialPosix : public aruna::drivers::comm::CommDriver {
            public:
                SerialPosix(char *port, uint32_t braudrate);

                err_t transmit(uint8_t *package, uint8_t package_size);

                char *getName();

                bool isEndpointConnected();

                bool isHardwareConnected();

                err_t start();

                err_t stop();

            private:
                bool port_opened = false;
                int file_description;
                const char *PORT;
                const uint32_t BRAUDRATE;
                pthread_t watch_thread;
                aruna::log::channel_t *log;

                void _watch_port_task();

                static void *watch_port_task(void *_this);
            };

        }}}
#endif //ARUNA_SERIALPOSIX_H
