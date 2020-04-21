//
// Created by noeel on 2-7-19.
//
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)) // System is Posix

#include <termio.h>
#include <fcntl.h>
#include <unistd.h>
#include "aruna/arunaTypes.h"
#include "aruna/comm/portable/posix/Serial.h"
#include <aruna/comm.h>
#include <aruna/log.h>
using namespace aruna::comm;

namespace aruna { namespace comm {

            err_t Serial::transmit(uint8_t *package, uint8_t package_size) {
                uint16_t bytes_written = 0;
                log->verbose("transmit size: %i", package_size);
                log->verbose("from: %i, to: %i, n: %i", package[2], package[3], package[1]);
                log->dump(aruna::log::level_t::VERBOSE, package, package_size);

                bytes_written = write(file_description, package, package_size);
                if (bytes_written != package_size) {
                    log->error("failed to write to port");
                    return err_t::HARDWARE_FAILURE;
                }
                return err_t::OK;

            }

            char *Serial::getName() {
                return (char *) "PosixSerial";
            }

            Serial::Serial(char *port, uint32_t braudrate) : PORT(port), BRAUDRATE(braudrate) {
                this->log = new aruna::log::channel_t("Serial");
            }

            bool Serial::isEndpointConnected() {
                return true;
            }

            bool Serial::isHardwareConnected() {
                return this->port_opened;
            }

            err_t Serial::start() {

                file_description = open(PORT, O_RDWR | O_NOCTTY | O_NDELAY);

                void *_this = this;
                if (file_description == -1) {
                    /*
                     * Could not open the port.
                     */
                    log->error("Unable to open port \"%s\": %s",PORT, strerror(errno));
                    port_opened = false;
                    return err_t::HARDWARE_FAILURE;
                }
                port_opened = true;
                fcntl(file_description, F_SETFL, FNDELAY);

                // Read the configureation of the port

                struct termios options;
                tcgetattr(file_description, &options);

                /* SEt Baud Rate */

                cfsetispeed(&options, BRAUDRATE);
                cfsetospeed(&options, BRAUDRATE);

                //I don't know what this is exactly

                options.c_cflag |= (CLOCAL | CREAD);

                // Set the Charactor size

                options.c_cflag &= ~CSIZE; /* Mask the character size bits */
                options.c_cflag |= CS8;    /* Select 8 data bits */

                // Set parity - No Parity (8N1)

                options.c_cflag &= ~PARENB;
                options.c_cflag &= ~CSTOPB;
                options.c_cflag &= ~CSIZE;
                options.c_cflag |= CS8;

                // Disable Hardware flowcontrol

//      options.c_cflag &= ~CNEW_RTSCTS;  -- not supported

                // Enable Raw Input

                options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

                // Disable Software Flow control

                options.c_iflag &= ~(IXON | IXOFF | IXANY);

                // Chose raw (not processed) output

                options.c_oflag &= ~OPOST;

                if (tcsetattr(file_description, TCSANOW, &options) == -1)
                    log->error("Error with tcsetattr = %s", strerror(errno));

                fcntl(file_description, F_SETFL, 0);

//    create watch task.
                pthread_create(&watch_thread, NULL, watch_port_task, _this);
                return err_t::OK;
            }

            err_t Serial::stop() {
                pthread_cancel(watch_thread);
                close(file_description);
                return err_t::OK;
            }

            void *Serial::watch_port_task(void *_this) {
//    static cast to make pthread happy
                ((Serial *) _this)->_watch_port_task();
                return nullptr;
            }

            void Serial::_watch_port_task() {
                uint16_t bytes_read = 0;
                uint16_t size = 0;
                uint8_t *buff;
                while (1) {
//        read the size of the package
                    bytes_read = read(file_description, &size, 1);
                    if (bytes_read && size >= 2) {
                        log->verbose("new incoming connection, size: %i", size);
                        buff = (uint8_t *) malloc(size);
                        buff[0] = size;
                        bytes_read = read(file_description, &buff[1], (size - 1));
                        log->dump(aruna::log::level_t::VERBOSE, buff, size);

                        incoming_connection(buff, size);
//            cleanup
                        free(buff);
                        size = 0;
                        bytes_read = 0;
                    }
                }
            }
        }}
#endif // System is Posix
