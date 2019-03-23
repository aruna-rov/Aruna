//
// Created by noeel on 16-1-19.
//

#ifndef ARUNA_UART_H
#define ARUNA_UART_H

#include "ComDriver.h"
#include "driver/uart.h"

//#include <drivers/Com.h>

class UART: public ComDriver {


//    TODO overwrite?
    com_err transmit(com_transmitpackage_t package);
    void getName(char *buffer);
    char* getName();
    unsigned int getSpeed();
    com_link_t getLinkType();
    bool isEndpointConnected();
//    static com_err registerDriver();
//    static com_err registerd = registerDriver();

    com_err start();
    com_err stop();

private:
    static void handle_rx_task(void *arg);

    const static unsigned int TX_BUF_SIZE = 256;
    const static unsigned int RX_BUF_SIZE = 512;
    const static uart_port_t UART_NUM = UART_NUM_0;
    const static unsigned int BROAD_RATE = 921600;

    const static int TXD_PIN = UART_PIN_NO_CHANGE;
    const static int RXD_PIN = UART_PIN_NO_CHANGE;
    const static int RTS_PIN = UART_PIN_NO_CHANGE;
    const static int CTS_PIN = UART_PIN_NO_CHANGE;





};

#endif //ARUNA_UART_H
