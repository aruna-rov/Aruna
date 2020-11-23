//
// Created by noeel on 16-1-19.
//

#ifndef ARUNA_ESP32_UART_H
#define ARUNA_ESP32_UART_H

#include "aruna/driver/UART.h"
#include "aruna/arunaTypes.h"
#include "driver/uart.h"

namespace aruna {
    namespace driver {

        class ESP32_UART : public UART {
        public:
            /**
             * Create a uart driver with custom settings
             * @param UART_NUM, 0, 1 or 2
             * @param TXD_PIN, configurate uart pin.
             * @param RXD_PIN, configurate uart pin.
             * @param RTS_PIN, configurate uart pin.
             * @param CTS_PIN, configurate uart pin.
             * @param UART_CONFIG, uart_config_t object
             * @param UART_MODE, use to enable RS485
             * @param TX_BUF_SIZE, transmit buffer
             * @param RX_BUF_SIZE, receive buffer
             */
            ESP32_UART(uart_port_t UART_NUM,
                       int TXD_PIN,
                       int RXD_PIN,
                       int RTS_PIN,
                       int CTS_PIN,
                       uart_config_t UART_CONFIG,
                       uart_mode_t UART_MODE,
                       unsigned int TX_BUF_SIZE,
                       unsigned int RX_BUF_SIZE);


            ~ESP32_UART();

            uint32_t get_read_buffer_length() override;

            uint32_t get_baudrate() override;

            flowcontrol_t get_flowcontrol() override;

            parity_t get_parity() override;

            stop_bit_t get_stop_bit() override;

            word_length_t get_word_length() override;


        private:

            //    uart port, 0 is usb, 1 and 2 can be defined by pins
            const uart_port_t UART_NUM;
            bool software_flowcontrol = false;

            QueueHandle_t uart_queue;

            size_t _write(uint8_t *data, size_t dataSize) override;

            size_t _read(uint8_t *buffer, size_t length) override;

            uint32_t _set_baudrate(uint32_t new_baudrate) override;

            err_t _set_flowcontrol(flowcontrol_t new_flowcontrol) override;

            err_t _set_parity(parity_t parity_bit) override;

            err_t _set_stop_bit(stop_bit_t stop_bit) override;

            err_t _set_word_length(word_length_t word_length) override;

        };
    }
}
#endif //ARUNA_ESP32_UART_H
