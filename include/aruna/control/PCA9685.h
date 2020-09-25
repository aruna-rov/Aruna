//
// Created by noeel on 15-09-20.
//

#ifndef ARUNA_CONTROL_PCA9685_H
#define ARUNA_CONTROL_PCA9685_H

#include "aruna/control/Actuator.h"

namespace aruna {
    namespace control {
        class PCA9685: public Actuator {
        private:
            const axis_mask_t axis;
            const direction_t direction;
            const uint8_t led;
            const uint8_t i2c_address;
            const float min_duty_cycle_percentage;
            const float max_duty_cycle_percentage;
        public:
            axis_mask_t get_axis() override;
            err_t set(axis_mask_t axisMask, uint16_t speed, direction_t direction) override;
            err_t start() override;

//            TODO add frequency
            /**
             * PCA9685 I²C PWM driver
             * @param axis: supported axis masks
             * @param direction: supported directions
             * @param led: PCA9685 LED output number (0-15)
             * @param i2c_address: address of the PCA (default 0b1000000)
             * @param min_duty_cycle_percentage: minimal duty on percentage (default 0)
             * @param max_duty_cycle_percentage: maxumal duty on percentage (default 100)
             */
            PCA9685(axis_mask_t axis, direction_t direction, uint8_t led, uint8_t i2c_address, float min_duty_cycle_percentage = 0, float max_duty_cycle_percentage = 100);
        };

    }
}


#endif //ARUNA_CONTROL_PCA9685_H
