//
// Created by noeel on 06-02-20.
//

#ifndef ARUNA_ESP32_STEPPER_H
#define ARUNA_ESP32_STEPPER_H

#include <aruna/control/Stepper.h>

namespace aruna {
    namespace control {
//        TODO namespace in "portable" consistant
        namespace esp32 {
            class Stepper : public control::Stepper {
                using control::Stepper::Stepper;
                err_t init_pin(uint8_t pin_nr) override;

                err_t set_pin(uint8_t pin_nr, bool value) override;
                err_t clear_pin(uint8_t pin_nr) override;
            };

        }
    }
}

#endif //ARUNA_ESP32_STEPPER_H
