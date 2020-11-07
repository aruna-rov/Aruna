//
// Created by noeel on 07-11-20.
//

#ifndef ARUNA_ESP32_MCPWM_H
#define ARUNA_ESP32_MCPWM_H

#include "aruna/driver/Pwm.h"
#include <driver/mcpwm.h>
#include <aruna/log.h>

namespace aruna {
    namespace driver {
        class ESP32_MCPwm: Pwm {
        private:
            const gpio_num_t pin;
            const mcpwm_unit_t pwm_unit;
            const mcpwm_timer_t pwm_timer;
            const mcpwm_io_signals_t io_signal;
            const mcpwm_operator_t pwm_operator;
            const mcpwm_config_t pwm_config;

            static constexpr mcpwm_config_t DEFAULT_PWM_CONFIG = {
                    100,
                    0,
                    0,
                    MCPWM_DUTY_MODE_0,
                    MCPWM_UP_COUNTER,
            };

            err_t _set_frequency(uint32_t frequency_hz) override;

            err_t _set_duty(uint16_t duty) override;

        public:

            /**
             * ESP32 MCPWM
             * @param pin: gpio pin to use.
             * @param pwm_unit: unit 0 or 1
             * @param pwm_timer: timer to use 0-2
             * @param io_signal: MCPWM0A - MCPWM2B
             * @param pwm_operator: MCPWM_OPR_A - MCPWM_OPR_B
             * @param pwm_config: user configuration for the pwm controller.
             */
            ESP32_MCPwm(gpio_num_t pin,
                        mcpwm_unit_t pwm_unit,
                        mcpwm_timer_t pwm_timer,
                        mcpwm_io_signals_t io_signal,
                        mcpwm_operator_t pwm_operator,
                        mcpwm_config_t pwm_config = DEFAULT_PWM_CONFIG);
        };
    }
}



#endif //ARUNA_ESP32_MCPWM_H
