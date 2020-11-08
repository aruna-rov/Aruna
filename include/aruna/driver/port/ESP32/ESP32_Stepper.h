//
// Created by noeel on 06-02-20.
//

#ifndef ARUNA_ESP32_STEPPER_H
#define ARUNA_ESP32_STEPPER_H

#include <aruna/driver/Stepper.h>

namespace aruna {
    namespace driver {
        class ESP32_Stepper : public Stepper {

            err_t init_pin(uint8_t pin_nr) override;

            err_t set_pin(uint8_t pin_nr, bool value) override;

            err_t clear_pin(uint8_t pin_nr) override;
        };
    }
}

#endif //ARUNA_ESP32_STEPPER_H
