//
// Created by noeel on 16-1-19.
//

#ifndef ARUNA_UART_H
#define ARUNA_UART_H

#include "ComDriver.h"
//#include <drivers/Com.h>

class UART: public ComDriver {
//    TODO overwrite?
    com_err transmit(com_transmitpackage_t package);
    void getName(char *buffer);
    unsigned int getSpeed();
    com_link_t getLinkType();
    bool isEndpointConnected();
    void int_incoming_connection();
//    static com_err registerDriver();
//    static com_err registerd = registerDriver();
};

#endif //ARUNA_UART_H
