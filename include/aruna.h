//
// Created by noeel on 4-7-19.
//

#ifndef ARUNA_H
#define ARUNA_H

#if defined(ESP_PLATFORM)
    // System is ESP
    // include embedded files
    #include "aruna/arunaTypes.h"
    #include "aruna/log.h"
    #include "aruna/blinky.h"
    #include "aruna/comm.h"
    #include "aruna/control/controlTypes.h"
    #include "aruna/control.h"
    #include "aruna/comm/CommDriver.h"
    #include "aruna/comm/portable/esp32/UART.h"
    #include "aruna/control/DriverSet.h"
    #include "aruna/control/ControlActuatorDriver.h"
    #include "aruna/control/portable/esp32/Pwm.h"

#elif defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
    // System is Posix
    // only include communication abstraction
	#include "aruna/arunaTypes.h"
    #include "aruna/log.h"
    #include "aruna/comm/portable/posix/SerialPosix.h"
    #include "aruna/comm.h"
#else
    #error "System is not supported."
#endif


#endif //ARUNA_H
