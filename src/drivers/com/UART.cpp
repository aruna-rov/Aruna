//
// Created by noeel on 16-1-19.
//

#include <stdio.h>
#include "drivers/com/UART.h"

com_err UART::transmit(com_transmitpackage_t package) {
//    printf(package);
    printf("hello world!");
//    TODO error handeling
    return COM_OK;
}
void UART::getName(char *buffer) {
//    TODO testen
    char name[] = "UART";
    *buffer = *name;
}
unsigned int UART::getSpeed() {
//    TODO get braudrate dynamicly
    return (unsigned int) 115200 / 8;
}
com_link_t UART::getLinkType() {
    return COM_WIRED;
}
bool UART::isEndpointConnected() {
//    TODO endpoint detection
    return true;
};