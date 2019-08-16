//
// Created by noeel on 30-3-19.
//

#ifndef ARUNA_PWM_H
#define ARUNA_PWM_H

#include <aruna.h>
#include "aruna/controlTypes.h"
#include "aruna/arunaTypes.h"
#include "ControlActuatorDriver.h"
#include <driver/mcpwm.h>

namespace aruna { namespace drivers { namespace control {

class Pwm: public ControlActuatorDriver  {
private:
	const aruna::control::axis_mask_t axis;
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
    aruna::control::axis_mask_t get_axis() override;

    err_t set(aruna::control::axis_mask_t axisMask, uint16_t speed, aruna::control::direction_t direction) override;

    Pwm(aruna::control::axis_mask_t axis, gpio_num_t forward_pin, gpio_num_t backward_pin, mcpwm_unit_t pwm_unit, mcpwm_timer_t pwm_timer, mcpwm_io_signals_t io_signal_forward, mcpwm_io_signals_t io_signal_backward);
private:
};
}}}

#endif //ARUNA_PWM_H
