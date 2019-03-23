//
// Created by noeel on 23-3-19.
//

#ifndef ARUNA_BLINKY_H
#define ARUNA_BLINKY_H

#include "Com.h"

enum LED_STATUS {
    LED_ON,
    LED_OFF,
    LED_TOGGLE
};

// TODO documentation
void start_blinky_task(void* arg);

// TODO documentation
void set_led(LED_STATUS ls);

void blinky_com_handeler(com_transmitpackage_t tp);

#endif //ARUNA_BLINKY_H
