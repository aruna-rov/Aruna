//
// Created by noeel on 16-1-19.
//

#ifndef ARUNA_UART_H
#define ARUNA_UART_H

#include "ComDriver.h"
//#include <drivers/Com.h>

class UART: ComDriver {
    com_err transmit(com_transmitpackage_t package);
    void getName(char *buffer);
    unsigned int getSpeed();
    com_link_t getLinkType();
    bool isEndpointConnected();
};

// TODO UART registreren.
//com.register_candidate_driver(UART);
#endif //ARUNA_UART_H
