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
    #include "aruna/comm/commTypes.h"
    #include "aruna/comm.h"
    #include "aruna/control/controlTypes.h"
    #include "aruna/control.h"
    #include "aruna/comm/Link.h"
    #include "aruna/comm/portable/esp32/UART.h"
    #include "aruna/control/ActuatorSet.h"
    #include "aruna/control/Actuator.h"
    #include "aruna/control/Stepper.h"
    #include "aruna/control/portable/esp32/Stepper.h"
    #include "aruna/control/portable/esp32/Pwm.h"
    #include "aruna/control/portable/esp32/Dshot.h"
    #include "aruna/sis.h"
    #include "aruna/sis/portable/esp32/WaterSensor.h"

#elif defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
    // System is Posix
    // only include communication abstraction
	#include "aruna/arunaTypes.h"
    #include "aruna/log.h"
    #include "aruna/comm/commTypes.h"
    #include "aruna/comm/portable/posix/Serial.h"
    #include "aruna/comm.h"
#elif defined(STM32F1)
    #warning "In development!"
#else
    #error "System is not supported."
#endif



#endif //ARUNA_H
