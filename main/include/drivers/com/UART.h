//
// Created by noeel on 16-1-19.
//

#ifndef ARUNA_UART_H
#define ARUNA_UART_H

#include "ComDriver.h"
#include "driver/uart.h"


class UART: public ComDriver {

    com_err transmit(com_transmitpackage_t package) override;
    char* getName() override;
    unsigned int getSpeed() override;
    com_link_t getLinkType() override;
    bool isEndpointConnected() override;
    com_err start() override;
    com_err stop() override;

private:
    /**
     * task to handle the rx of uart, will run forever.
     * @param arg, not used but nessesary for RTOS.
     */
    static void handle_rx_task(void *arg);

//    buffer sizes
    const static unsigned int TX_BUF_SIZE = 256;
    const static unsigned int RX_BUF_SIZE = 512;

//    uart port, 0 is usb, 1 and 2 can be defined by pins
    const static uart_port_t UART_NUM = UART_NUM_0;

//    TODO increase speed to 2000000
    const static unsigned int BROAD_RATE = 921600;

//    uart pins, default use USB
    const static int TXD_PIN = UART_PIN_NO_CHANGE;
    const static int RXD_PIN = UART_PIN_NO_CHANGE;
    const static int RTS_PIN = UART_PIN_NO_CHANGE;
    const static int CTS_PIN = UART_PIN_NO_CHANGE;
};

#endif //ARUNA_UART_H
