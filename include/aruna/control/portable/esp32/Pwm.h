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
	aruna::log::channel_t log;
public:
    err_t start() override;
    err_t stop() override;
    axis_mask_t get_axis() override;

    err_t set(axis_mask_t axisMask, uint16_t speed, direction_t direction) override;

    Pwm(axis_mask_t axis, gpio_num_t forward_pin, gpio_num_t backward_pin, mcpwm_unit_t pwm_unit, mcpwm_timer_t pwm_timer, mcpwm_io_signals_t io_signal_forward, mcpwm_io_signals_t io_signal_backward);
private:
};
}}

#endif //ARUNA_PWM_H
