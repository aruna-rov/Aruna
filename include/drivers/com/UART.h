//
// Created by noeel on 16-1-19.
//

#ifndef ARUNA_UART_H
#define ARUNA_UART_H

// including comdriver breaks comdriver.h refrences to com.h
#include "ComDriver.h"

//#include <drivers/Com.h>

class UART: ComDriver {
    com_err transmit(com_transmitpackage_t package);
    void getName(char *buffer);
    unsigned int getSpeed();
    com_link_t getLinkType();
    bool isEndpointConnected();
};


#endif //ARUNA_UART_H
