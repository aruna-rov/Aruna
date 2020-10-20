//
// Created by noeel on 18-10-20.
//

#ifndef ARUNA_PWM_H
#define ARUNA_PWM_H

#include "aruna/arunaTypes.h"

namespace aruna {
    namespace driver {
        class Pwm {
        private:
            uint32_t frequency_hz;
            uint16_t duty;

            virtual err_t _set_frequency(uint32_t frequency_hz) = 0;

            virtual err_t _set_duty(uint16_t duty) = 0;

        public:

            /**
             * Set frequency of PWM in hertz.
             * @param frequency_hz: frequency for pwm (might be set on multiple pwm objects)
             * @return unspecified aruna error
             */
            err_t set_frequency(uint32_t frequency_hz);

            /**
             * Get the frequency used in hertz
             * @return frequency in hertz
             */
            uint32_t get_frequency() const;

            /**
             * Set the duty cycle on time in percentage.
             * @param duty_percentage: value from 0 to 100
             * @return err_t::OK if it whent well.
             */
            err_t set_duty(float duty_percentage);

            /**
             * Set the duty cycle on time (16bit).
             * @param duty: pwm on time
             * @return err_t::OK if successfull.
             */
            err_t set_duty(uint16_t duty);

            /**
             * Get the duty cycle on time in percentage
             * @return on time (0-100)
             */
            float get_duty_per() const;

            /**
             * Get the duty cycle on time (16bit)
             * @return on time (16bit)
             */
            uint16_t get_duty() const;

        };
    }
}


#endif //ARUNA_PWM_H
