//
// Created by noeel on 15-09-20.
//

#ifndef ARUNA_PCA9685_H
#define ARUNA_PCA9685_H

#include "PCA9685_map.h"
#include "aruna/arunaTypes.h"

namespace aruna {
    namespace driver {
        namespace PCA9685 {

            /**
             * Set the PWM of an output of the PCA9685
             * @param led, 0-15 output to adjust
             * @param on, 12bit time to turn on
             * @param off, 12bit time to turn off
             * @param address, I²C address of device, default 0b1000000
             * @return
             */
            err_t set_pwm(uint8_t led, uint16_t on, uint16_t off, uint8_t address = default_address);

        }
    }
}
#endif //ARUNA_PCA9685_H
