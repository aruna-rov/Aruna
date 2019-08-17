//
// Created by noeel on 16-1-19.
//

#ifndef ARUNA_UART_H
#define ARUNA_UART_H

#include "aruna/comm/CommDriver.h"
#include "aruna/arunaTypes.h"
#include "driver/uart.h"

namespace aruna { namespace comm {

class UART: public CommDriver {
public:
/**
 * Create a uart driver with custom settings
 * @param TAG, name of the driver
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
    UART(char *TAG,
         uart_port_t UART_NUM,
         int TXD_PIN,
         int RXD_PIN,
         int RTS_PIN,
         int CTS_PIN,
         uart_config_t UART_CONFIG,
         uart_mode_t UART_MODE,
         unsigned int TX_BUF_SIZE,
         unsigned int RX_BUF_SIZE);
    /**
     * Default uart constructor. Uses braudrate 921600 over usb, parity even.
     */
    UART();
    err_t transmit(uint8_t *package, uint8_t package_size) override;
    char* getName() override;
    unsigned int getSpeed() override;
    link_t getLinkType() override;
    bool isEndpointConnected() override;
    err_t start() override;
    err_t stop() override;

private:
    /**
     * task to handle the rx of uart, will run forever.
     * @param __this, pass object of uart class.
     */
    static void handle_rx_task(void *__this);

    const char *TAG = "UART";

//    uart port, 0 is usb, 1 and 2 can be defined by pins
    const uart_port_t UART_NUM = UART_NUM_0;

//    uart pins, default use USB
    const int TXD_PIN = UART_PIN_NO_CHANGE;
    const int RXD_PIN = UART_PIN_NO_CHANGE;
    const int RTS_PIN = UART_PIN_NO_CHANGE;
    const int CTS_PIN = UART_PIN_NO_CHANGE;

    const uart_config_t UART_CONFIG = {
    //    TODO increase speed to 2000000
            .baud_rate = 921600,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .rx_flow_ctrl_thresh = 122,
            .use_ref_tick = false
    };

    const uart_mode_t UART_MODE = UART_MODE_UART;

    //    buffer sizes
    const unsigned int TX_BUF_SIZE = 256;
    const unsigned int RX_BUF_SIZE = 512;

    QueueHandle_t uart_queue;
    bool installed = false;
    xTaskHandle uart_rx_handle = nullptr;
};
}}
#endif //ARUNA_UART_H
