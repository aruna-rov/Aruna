//
// Created by noeel on 16-1-19.
//

#include <stdio.h>
#include "drivers/com/UART.h"
#include <drivers/Com.h>
#include <drivers/com/UART.h>
#include <esp_log.h>


com_err UART::transmit(com_transmitpackage_t package) {
//    printf(package);
    printf("hello world!");
//    TODO error handeling
    return COM_OK;
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
}

void UART::int_incoming_connection() {
//    TODO
}

char *UART::getName() {
    return (char*) "UART";
}

//com_err UART::registerDriver() {
//    ESP_LOGD("UART", "registering driver!");
//    ComDriver * driver = this;
////    TODO error handeling.
//    return this->registerd = COM.register_candidate_driver(driver);
//};

