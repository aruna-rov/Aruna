//
// Created by noeel on 16-1-19.
//

#include <stdio.h>
#include "aruna/arunaTypes.h"
#include <aruna/comm.h>
#include "aruna/driver/port/ESP32/ESP32_UART.h"
#include "esp_log.h"


namespace aruna {
    namespace driver {

        ESP32_UART::ESP32_UART(uart_port_t UART_NUM = UART_NUM_0,
                               int TXD_PIN = UART_PIN_NO_CHANGE,
                               int RXD_PIN = UART_PIN_NO_CHANGE,
                               int RTS_PIN = UART_PIN_NO_CHANGE,
                               int CTS_PIN = UART_PIN_NO_CHANGE,
                               uart_config_t UART_CONFIG = (uart_config_t) {
                                       .baud_rate = 115200,
                                       .data_bits = UART_DATA_8_BITS,
                                       .parity    = UART_PARITY_DISABLE,
                                       .stop_bits = UART_STOP_BITS_1,
                                       .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                                       .rx_flow_ctrl_thresh = 122,
                                       .use_ref_tick = false
                               },
                               uart_mode_t UART_MODE = UART_MODE_UART,
                               unsigned int TX_BUF_SIZE = 256,
                               unsigned int RX_BUF_SIZE = 512) : UART_NUM(UART_NUM) {
            //    TODO logging zou moeten worden uitgeschakeld op de target UART.

            /* Configure parameters of an UART driver,
             * communication pins and install the driver */
//        set config
            ESP_ERROR_CHECK(uart_param_config(UART_NUM, &UART_CONFIG));
//        set pins
            ESP_ERROR_CHECK(uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, RTS_PIN, CTS_PIN));
//        install driver (driver can only be installed once, even if you uart_driver_delete is used)
            ESP_ERROR_CHECK(uart_driver_install(UART_NUM, RX_BUF_SIZE, TX_BUF_SIZE, 20, &uart_queue, 0));
//        Set correct mode
            ESP_ERROR_CHECK(uart_set_mode(UART_NUM, UART_MODE));
        }

        size_t ESP32_UART::_write(uint8_t *data, size_t dataSize) {
            return uart_write_bytes(UART_NUM, (const char *) data, dataSize);
        }

        unsigned int ESP32_UART::get_baudrate() {
            uint32_t br = 0;
            uart_get_baudrate(UART_NUM, &br);
            return br;
        }

        ESP32_UART::~ESP32_UART() {
            //    TODO flush tx queue
            //  flush rx queue
            xQueueReset(uart_queue);
            ESP_ERROR_CHECK(uart_flush_input(UART_NUM));

            /*
            *      new driver cannot be installed even if uart_delete_driver is called.
            *      this causes an issue in driver selection because the winning driver gets restarted
            */
            ESP_ERROR_CHECK(uart_driver_delete(UART_NUM));
        }

        size_t ESP32_UART::_read(uint8_t *buffer, size_t length) {
            return uart_read_bytes(this->UART_NUM, buffer, length, 4);
        }

        uint32_t ESP32_UART::_set_baudrate(uint32_t new_baudrate) {
            ESP_ERROR_CHECK(uart_set_baudrate(UART_NUM, new_baudrate));
            return get_baudrate();
        }

        err_t ESP32_UART::_set_flowcontrol(UART::flowcontrol_t new_flowcontrol) {
            esp_err_t hw, sw;
            err_t err = err_t::OK;
            switch (new_flowcontrol) {
                case flowcontrol_t::HARDWARE:
                    hw = uart_set_hw_flow_ctrl(UART_NUM, UART_HW_FLOWCTRL_CTS_RTS, 0);
                    sw = uart_set_sw_flow_ctrl(UART_NUM, false, XON, XOFF);
                    software_flowcontrol = false;
                    break;
                case flowcontrol_t::SOFTWARE:
                    hw = uart_set_hw_flow_ctrl(UART_NUM, UART_HW_FLOWCTRL_DISABLE, 0);
                    sw = uart_set_sw_flow_ctrl(UART_NUM, true, XON, XOFF);
                    software_flowcontrol = true;
                    break;
                case flowcontrol_t::HARDWARE_SOFTWARE:
                    hw = uart_set_hw_flow_ctrl(UART_NUM, UART_HW_FLOWCTRL_CTS_RTS, 0);
                    sw = uart_set_sw_flow_ctrl(UART_NUM, true, XON, XOFF);
                    software_flowcontrol = true;
                    break;
                case flowcontrol_t::NONE:
                case flowcontrol_t::DISABLED:
                default:
                    hw = uart_set_hw_flow_ctrl(UART_NUM, UART_HW_FLOWCTRL_DISABLE, 0);
                    sw = uart_set_sw_flow_ctrl(UART_NUM, false, XON, XOFF);
                    software_flowcontrol = false;
                    break;
            }
            err = hw || sw ? err_t::FAIL : err_t::OK;

            return err;
        }

        err_t ESP32_UART::_set_parity(UART::parity_t parity_bit) {
            uart_parity_t esp_parity;
            switch (parity_bit) {
                case parity_t::EVEN:
                    esp_parity = UART_PARITY_EVEN;
                    break;
                case parity_t::ODD:
                    esp_parity = UART_PARITY_ODD;
                    break;
                case parity_t::DISABLED:
                default:
                    esp_parity = UART_PARITY_DISABLE;
                    break;
            }
            if (uart_set_parity(UART_NUM, esp_parity))
                return err_t::INVALID_PARAMETERS;
            else
                return err_t::OK;
        }

        err_t ESP32_UART::_set_stop_bit(UART::stop_bit_t stop_bit) {
//            TODO maak een map of bimap
//https://stackoverflow.com/questions/21760343/is-there-a-more-efficient-implementation-for-a-bidirectional-map
            uart_stop_bits_t esp_stop;
            switch (stop_bit) {
                case stop_bit_t::ONE_HALF:
                    esp_stop = UART_STOP_BITS_1_5;
                    break;
                case stop_bit_t::TWO:
                    esp_stop = UART_STOP_BITS_2;
                    break;
                case stop_bit_t::ONE:
                default:
                    esp_stop = UART_STOP_BITS_1;
                    break;

            }
            if (uart_set_stop_bits(UART_NUM, esp_stop))
                return err_t::FAIL;
            else
                return err_t::OK;
        }

        uint32_t ESP32_UART::get_read_buffer_length() {
            size_t len = 0;
            ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_NUM, &len));
            return len;
        }

        UART::flowcontrol_t ESP32_UART::get_flowcontrol() {
            uart_hw_flowcontrol_t esp_hw;
            flowcontrol_t aruna_fc;
            ESP_ERROR_CHECK(uart_get_hw_flow_ctrl(UART_NUM, &esp_hw));
            switch (esp_hw) {
                case UART_HW_FLOWCTRL_DISABLE:
                    aruna_fc = flowcontrol_t::DISABLED;
                    break;
                case UART_HW_FLOWCTRL_RTS:
                case UART_HW_FLOWCTRL_CTS:
                case UART_HW_FLOWCTRL_CTS_RTS:
                    aruna_fc = flowcontrol_t::HARDWARE;
                    break;
                case UART_HW_FLOWCTRL_MAX:
                default:
                    aruna_fc = flowcontrol_t::NONE;
                    break;
            }
            if (software_flowcontrol && aruna_fc == flowcontrol_t::HARDWARE)
                aruna_fc = flowcontrol_t::HARDWARE_SOFTWARE;
            else if (software_flowcontrol)
                aruna_fc = flowcontrol_t::SOFTWARE;

            return aruna_fc;
        }

        UART::parity_t ESP32_UART::get_parity() {
            uart_parity_t esp_parity;
            parity_t aruna_parity;
            ESP_ERROR_CHECK(uart_get_parity(UART_NUM, &esp_parity));
            switch (esp_parity) {
                case UART_PARITY_EVEN:
                    aruna_parity = parity_t::EVEN;
                    break;
                case UART_PARITY_ODD:
                    aruna_parity = parity_t::ODD;
                    break;
                case UART_PARITY_DISABLE:
                default:
                    aruna_parity = parity_t::DISABLED;
                    break;
            }
            return aruna_parity;
        }

        UART::stop_bit_t ESP32_UART::get_stop_bit() {
            uart_stop_bits_t esp_stop;
            stop_bit_t aruna_stop;
            ESP_ERROR_CHECK(uart_get_stop_bits(UART_NUM, &esp_stop));
            switch (esp_stop) {
                case UART_STOP_BITS_1:
                    aruna_stop = stop_bit_t::ONE;
                    break;
                case UART_STOP_BITS_1_5:
                    aruna_stop = stop_bit_t::ONE_HALF;
                    break;
                case UART_STOP_BITS_2:
                    aruna_stop = stop_bit_t::TWO;
                    break;
                case UART_STOP_BITS_MAX:
                default:
                    aruna_stop = stop_bit_t::NONE;
                    break;
            }

            return aruna_stop;
        }

        UART::word_length_t ESP32_UART::get_word_length() {
            uart_word_length_t esp_word;
            word_length_t aruna_word;
            ESP_ERROR_CHECK(uart_get_word_length(UART_NUM, &esp_word));
            switch (esp_word) {
                case UART_DATA_5_BITS:
                    aruna_word = word_length_t::FIVE;
                    break;
                case UART_DATA_6_BITS:
                    aruna_word = word_length_t::SIX;
                    break;
                case UART_DATA_7_BITS:
                    aruna_word = word_length_t::SEVEN;
                    break;
                case UART_DATA_8_BITS:
                    aruna_word = word_length_t::EIGHT;
                    break;
                case UART_DATA_BITS_MAX:
                default:
                    aruna_word = word_length_t::NONE;
                    break;
            }
            return aruna_word;
        }

        err_t ESP32_UART::_set_word_length(UART::word_length_t word_length) {
            uart_word_length_t esp_word;
            switch (word_length) {
                case word_length_t::FIVE:
                    esp_word = UART_DATA_5_BITS;
                    break;
                case word_length_t::SIX:
                    esp_word = UART_DATA_6_BITS;
                    break;
                case word_length_t::SEVEN:
                    esp_word = UART_DATA_7_BITS;
                    break;
                case word_length_t::EIGHT:
                default:
                    esp_word = UART_DATA_8_BITS;
                    break;
            }
            esp_err_t esp_err = uart_set_word_length(UART_NUM, esp_word);
            err_t err;
            err = esp_err ? err_t::FAIL : err_t::OK;
            return err;
        }
    }
}
