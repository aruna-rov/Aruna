//
// Created by noeel on 7-1-19.
//

#ifndef ARUNA_COMM_H
#define ARUNA_COMM_H

#include "aruna/arunaTypes.h"
#include <aruna/comm/commTypes.h>
#include <aruna/comm/Link.h>

namespace aruna {

    namespace comm {
        // control functions

        /**
         * @brief  Start new communication. Using `LINK_HARDWARE` to define hardware
         * @retval err_t
         *  * `HARDWARE_ERROR` if the hardware fails.
         *  * `OK` great success!
         */
        err_t start();

        /**
         * @brief  Start new communication. Using `LINK_HARDWARE` to define hardware
         * @param driver force to use specific driver.
         * @retval err_t
         *  * `HARDWARE_ERROR` if the hardware fails.
         *  * `OK` great success!
         */
        err_t start(Link *driver);

        /**
         * @brief  Stop the communication, free all queue's, channels and buffers
         * @retval err_t
         *  * `HARDWARE_ERROR` if the hardware fails.
         *  * `OK` great success!
         *  * `NOT_STARTED` if the comm was not started.
         */
        err_t stop();

        /**
         * @brief  Register a new communication endpoint
         * @param  channel: channel_t object
         * @retval err_t
         *  * `INVALID_PARAMETERS` if parameters are invalid
         *  * `OK` if it was succesfully added.
         *  * `CHANNEL_EXISTS` if the channel already exists.
         *  * `BUFFER_OVERFLOW` channel buffer overflow
         */
        err_t register_channel(channel_t *channel);

        /**
         * @brief  unregister an endpoint
         * @param  channel: endpoint to be removed
         * @retval err_t
         *  * `OK` if it was succesfully removed.
         *  * `NO_CHANNEL` ain't got no channel, ain't got no knife.
         */
        err_t unregister_channel(channel_t &channel);

        /**
         * @brief  Send data.
         * @note
         * @param channel: channel handler that is sending the data
         * @param to_port: to which port to send de data to.
         * @param  data: Data to send.
         * @param data_size: length of the data
         * @param wait_for_ack: if set to thrue, thread will block until ack is received or until timeout is reached
         * @retval err_t
         *  * `INVALID_PARAMETERS` if parameters are invalid
         *  * `OK` if it was succesfully send.
         *  * `NO_CONNECTION` if there is no connection,
         *  * `BUFFER_OVERFLOW` if the data was not added to the bugger due an overflow,
         *  * `NO_CHANNEL` if the channel does'nt exist.
         *  * `NO_RESPONSE` if there was no response (only if `wait_for_ack` is true)
         */
        //	 TODO documentation
        err_t send(channel_t *channel, port_t to_port, uint8_t *data, size_t data_size, bool wait_for_ack = false);

        /**
         * pause all communication. buffers, channels and queue's will be saved
         * @return err_t
         *  * `OK` if it was a success,
         *  * `NOT_STARTED` if the comm was not started.
         */
        err_t pause();

        /**
         * resume all communication.
         * @return err_t
         *  * `OK` if it was a success,
         *  * `NOT_STARTED` if the comm was not started.
         *  * `NOT_PAUSED` if it was not paused.
         */
        err_t resume();

        // getters and setters

        /**
         * @brief  Get the status of the comm link
         * @retval 1 is connected, 0 if not.
         */
        bool is_connected();

        /**
         * Get the running status of com
         * @return `status_t` (RUNNING, PAUSED, STOPPED)
         */
        status_t get_status();

        /**
         * @brief  Get connection hardware type
         * @retval link_t
         */
        link_t get_link_type();

        /**
         * get the name of the driver.
         * @param buffer char* to put the name into
         */
        void getName(char *buffer);

        /**
         * get the name of the driver
         * @return char*, name of the driver.
         */
        char *getName();

        /**
         * @brief get all names of the channels currently registered
         * @param buffer to write the names into.
         * @return err_t
         *  * `OK` if it was a success
         *  * `BUFFER_OVERFLOW` if the suplied buffer is to small,
         *  * `INVALID_PARAMETERS if the parameters are incorrect,
         */
        err_t get_channels(char *buffer);

        /**
         * @brief  Get the speed of the link to the other hosts in kB/s
         * @note   returns 0 if there is no connection.
         * @retval int, speed
         */
        unsigned int get_speed();

        /**
         * register a driver to be a comm driver candidate.
         * @param driver ComDriver object
         * @return err_t
         * * `DRIVER_EXISTS` if driver already exists.
         * * `BUFFER_OVERFLOW` driver buffer overflow
         * * `OK` all is well :).
         */
        err_t register_candidate_driver(Link *driver);

        /**
         * unregister a driver to be a comdiver candidate
         * @param driver ComDriver to be deleted
         * @return err_t
         * * `OK` great success!
         * * `NO_DRIVER` driver does'nt exists.
         */
        err_t unregister_candidate_driver(Link *driver);

        /**
         * get all the comm driver candidates
         */
        void get_candidate_drivers(char *buffer[]);
        //     TODO come here and make it!

        /**
         * @brief  Interrupt incomming connection handeler
         * @param package that needs to be handeled.
         * @retval err_t
         * `NOT_STARTED` comm is not started
         * `OK` handeled
         * `NO_CHANNEL` there is no channel to handle it
         */
        err_t incoming_connection(uint8_t *package, uint8_t package_size);

    }
}
#endif //ARUNA_COMM_H
