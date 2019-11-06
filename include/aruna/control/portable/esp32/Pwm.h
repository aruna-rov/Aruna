//
// Created by noeel on 30-3-19.
//

#ifndef ARUNA_PWM_H
#define ARUNA_PWM_H

#include <aruna.h>
#include "aruna/control/controlTypes.h"
#include "aruna/arunaTypes.h"
#include "aruna/control/Actuator.h"
#include <driver/mcpwm.h>

namespace aruna { namespace control {

class Pwm: public Actuator  {
private:
	const axis_mask_t axis;
	const gpio_num_t forward_pin;
	const gpio_num_t backward_pin;
	const mcpwm_unit_t pwm_unit;
	const mcpwm_timer_t pwm_timer;
	const mcpwm_io_signals_t io_signal_forward;
	const mcpwm_io_signals_t io_signal_backward;
    const mcpwm_operator_t pwm_operator;
	const mcpwm_config_t pwm_config;
	const float min_duty_cycle_percentage;
	const float max_duty_cycle_percentage;
	const direction_t direction;
	const uint8_t pwm_num;
	aruna::log::channel_t log;
public:
    static const mcpwm_config_t DEFAULT_PWM_CONFIG;
    err_t start() override;
    err_t stop() override;
    axis_mask_t get_axis() override;

    err_t set(axis_mask_t axisMask, uint16_t speed, direction_t direction) override;

//    double pwm
    Pwm(axis_mask_t axis,
            gpio_num_t forward_pin,
            gpio_num_t backward_pin,
            mcpwm_unit_t pwm_unit,
            mcpwm_timer_t pwm_timer,
            mcpwm_io_signals_t io_signal_forward,
            mcpwm_io_signals_t io_signal_backward,
            mcpwm_config_t pwm_config = DEFAULT_PWM_CONFIG,
            float min_duty_cycle_percentage = 0,
            float max_duty_cycle_percentage = 100);

//    single pwm
    Pwm(axis_mask_t axis,
            gpio_num_t pin,
            mcpwm_unit_t pwm_unit,
            mcpwm_timer_t pwm_timer,
            mcpwm_io_signals_t io_signal,
            mcpwm_operator_t pwm_operator,
            direction_t direction,
            mcpwm_config_t pwm_config = DEFAULT_PWM_CONFIG,
            float min_duty_cycle_percentage = 0,
            float max_duty_cycle_percentage = 100);

private:
};
}}

#endif //ARUNA_PWM_H
