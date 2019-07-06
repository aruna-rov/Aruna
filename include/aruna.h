//
// Created by noeel on 4-7-19.
//

#ifndef ARUNA_H
#define ARUNA_H

#if  defined(ESP_PLATFORM)
    // System is ESP
    // include embedded files
    #include "aruna/blinky.h"
    #include "aruna/Com.h"
    #include "aruna/control.h"
    #include "aruna/drivers/com/ComDriver.h"
    #include "aruna/drivers/com/UART.h"
    #include "aruna/drivers/control/ControlActuatorDriver.h"
    #include "aruna/drivers/control/L293D.h"

#elif defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
    // System is Posix
    // only include communication abstraction
    #include "Com.h"
#else
    #error "System is not supported."
#endif


#endif //ARUNA_H
