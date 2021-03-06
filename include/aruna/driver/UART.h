//
// Created by noeel on 16-11-20.
//

#ifndef ARUNA_UART_H
#define ARUNA_UART_H

#include <pthread.h>
#include <cstdint>
#include <aruna/comm/Link.h>
#include "aruna/arunaTypes.h"

namespace aruna {
    namespace driver {
        class UART: public aruna::comm::Link {
        public:
            enum class flowcontrol_t {
                HARDWARE,
                SOFTWARE,
                HARDWARE_SOFTWARE,
                DISABLED,
                NONE,
            };
            enum class parity_t {
                EVEN,
                ODD,
                DISABLED,
                NONE,
            };
            enum class stop_bit_t {
                ONE,
                ONE_HALF,
                TWO,
                NONE,
            };
            enum class word_length_t {
                FIVE,
                SIX,
                SEVEN,
                EIGHT,
                NONE
            };
        private:
            uint32_t baudrate = 0;
            flowcontrol_t flowcontrol = flowcontrol_t::NONE;
            parity_t parity = parity_t::NONE;
            stop_bit_t stopBit = stop_bit_t::NONE;
            word_length_t wordLength = word_length_t::NONE;
            pthread_mutex_t write_atomic;
            pthread_mutex_t read_atomic;

//            TODO _read and _write are no exposed by comm::Link::transmit/receive. No so safe! Can be fixed with an IO abstraction layer
            /**
             * Set baudrate of the UART
             * @param new_baudrate: baudrate to change to.
             * @return new baudrate
             */
            virtual uint32_t _set_baudrate(uint32_t new_baudrate) = 0;

            /**
             * Set software or hardware flowcontrol
             * @param new_flowcontrol
             * @return err_t::OK if set correctly
             */
            virtual err_t _set_flowcontrol(flowcontrol_t new_flowcontrol) = 0;

            /**
             * Set parity bit
             * @param parity_bit: EVEN, UNEVEN or DISABLED
             * @return err_t::OK if no errors.
             */
            virtual err_t _set_parity(parity_t parity_bit) = 0;

            /**
             * set stop bit
             * @param stop_bit: 1 1.5 or 2 bits
             * @return err_t::OK if no errors
             */
            virtual err_t _set_stop_bit(stop_bit_t stop_bit) = 0;

            /**
             * Set UART data bits length
             * @param word_length: five to eight bits
             * @return err_t::OK if success.
             */
            virtual err_t _set_word_length(word_length_t word_length) = 0;

        protected:

            const static uint8_t XON = 0x11;
            const static uint8_t XOFF = 0x13;
        public:

            /**
             * UART object to write and read data.
             */
            UART();

            ~UART();

            /**
             * Write data to the UART.
             * @param data: pointer to array of bytes to be written.
             * @param dataSize: length of data
             * @return amount of bytes written.
             */
            size_t write(uint8_t *data, size_t dataSize);

            /**
             * Write data to the UART. Return if data is already being written.
             * @param data: pointer to array of bytes to be written.
             * @param dataSize: length of data
             * @return amount of bytes written.
             */
            size_t try_write(uint8_t *data, size_t dataSize);
//            TODO add timed_write with mutex timeout.
//            TODO add (try_)write/read and lock to IO abstraction layer.
            /**
             * read data from the UART.
             * @param buffer: pointer to buffer where UART read data will be written.
             * @param length: amount of bytes to read.
             * @return amount of bytes read.
             */
            size_t read(uint8_t *buffer, size_t length);

            /**
             * read data from the UART. Return of data os already being read.
             * @param buffer: pointer to buffer where UART read data will be written.
             * @param length: amount of bytes to read.
             * @return amount of bytes read.
             */
            size_t try_read(uint8_t *buffer, size_t length);
//            TODO add timed_read with mutex timeout.

            /**
             * Get amount of bytes in the read buffer.
             * @return number of bytes in read buffer.
             */
            virtual uint32_t get_read_buffer_length() = 0;
//            TODO get total buffer length.

            /**
             * Set baudrate of the UART
             * @param new_baudrate: baudrate to change to.
             * @return new baudrate
             */
            uint32_t set_baudrate(uint32_t new_baudrate);

            /**
             * Get baudrate of UART
             * @return baudrate
             */
            virtual uint32_t get_baudrate();

            /**
             * Set software or hardware flowcontrol
             * @param new_flowcontrol
             * @return err_t::OK if set correctly
             */
            err_t set_flowcontrol(flowcontrol_t new_flowcontrol);

            /**
             * get flowcontrol of UART
             * @return flowcontrol
             */
            virtual flowcontrol_t get_flowcontrol();

            /**
             * Set parity bit
             * @param parity_bit: EVEN, UNEVEN or DISABLED
             * @return err_t::OK if no errors.
             */
            err_t set_parity(parity_t parity_bit);

            /**
             * Get parity of UART
             * @return parity bit setting.
             */
            virtual parity_t get_parity();

            /**
             * set stop bit
             * @param stop_bit: 1 1.5 or 2 bits
             * @return err_t::OK if no errors
             */
            err_t set_stop_bit(stop_bit_t stop_bit);

            /**
             * get stop bit
             * @return ONE ONE_HALF or TWO
             */
            virtual stop_bit_t get_stop_bit();

            /**
             * Set UART data bits length
             * @param word_length: five to eight bits
             * @return err_t::OK if success.
             */
            err_t set_word_length(word_length_t word_length);

            /**
             * Set UART data bits length
             * @return word length.
             */
            virtual word_length_t get_word_length();

            uint32_t get_speed() override;

//            TODO add support to lock UART, simular to I2C address locking.
//            TODO add support for interupts. Simulair to https://github.com/espressif/esp-idf/blob/master/examples/peripherals/uart/uart_events/main/uart_events_example_main.c





        };
    }
}


#endif //ARUNA_UART_H
