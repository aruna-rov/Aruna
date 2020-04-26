//
// Created by noeel on 30-3-19.
//

#include <driver/mcpwm.h>
#include "aruna/control/portable/esp32/Pwm.h"
#include "aruna/control.h"
namespace aruna { namespace control {
    const mcpwm_config_t Pwm::DEFAULT_PWM_CONFIG = {
        100,
        0,
        0,
        MCPWM_DUTY_MODE_0,
        MCPWM_UP_COUNTER,
};

axis_mask_t Pwm::get_axis() {
    return axis;
}

err_t Pwm::start() {
    ESP_ERROR_CHECK(mcpwm_gpio_init(pwm_unit, io_signal_forward, forward_pin));
    if (pwm_num > 1)
	    ESP_ERROR_CHECK(mcpwm_gpio_init(pwm_unit, io_signal_backward, backward_pin));


//  TODO what happens if mcpwm_init runs two times?
    ESP_ERROR_CHECK(mcpwm_init(pwm_unit, pwm_timer, &pwm_config));
    set(this->get_axis(), 0, this->direction);
    return err_t::OK;
}

err_t Pwm::set(axis_mask_t axisMask, uint16_t speed, direction_t direction) {
	log.debug("Pwm", "axis:%X, speed:%d, dir:%d", (int) axisMask, speed, (int) direction);
	if ((uint8_t) axisMask & (uint8_t) axis && (this->direction == direction_t::BOTH || this->direction == direction)) {
		float per_up = convert_range(speed, max_duty_cycle_percentage, min_duty_cycle_percentage);
        mcpwm_operator_t low;
        mcpwm_operator_t high;
		if (pwm_num > 1) {
            low = (bool) direction ? MCPWM_OPR_A : MCPWM_OPR_B;
            high = (bool) direction ? MCPWM_OPR_B : MCPWM_OPR_A;

            ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_unit, pwm_timer, low));
        } else
            high = pwm_operator;

		ESP_ERROR_CHECK(mcpwm_set_duty(pwm_unit, pwm_timer, high, per_up));

		ESP_ERROR_CHECK(mcpwm_set_duty_type(pwm_unit, pwm_timer, high, MCPWM_DUTY_MODE_0));
	}
	return err_t::OK;
}

//  single pwm
        Pwm::Pwm(axis_mask_t axis,
                 gpio_num_t pin,
                 mcpwm_unit_t pwm_unit,
                 mcpwm_timer_t pwm_timer,
                 mcpwm_io_signals_t io_signal,
                 mcpwm_operator_t pwm_operator,
                 direction_t direction,
                 mcpwm_config_t pwm_config,
                 float min_duty_cycle_percentage,
                 float max_duty_cycle_percentage):

                 axis(axis),
                 forward_pin(pin),
                 backward_pin( gpio_num_t (NULL)),
                 pwm_unit(pwm_unit),
                 pwm_timer(pwm_timer),
                 io_signal_forward(io_signal),
                 io_signal_backward(mcpwm_io_signals_t (NULL)),
                 pwm_operator(pwm_operator),
                 pwm_config(pwm_config),
                 min_duty_cycle_percentage(min_duty_cycle_percentage),
                 max_duty_cycle_percentage(max_duty_cycle_percentage),
                 direction(direction),
                 pwm_num(1),
                 log("Pwm") {

        }
// double pwm
        Pwm::Pwm(axis_mask_t axis,
                 gpio_num_t forward_pin,
                 gpio_num_t backward_pin,
                 mcpwm_unit_t pwm_unit,
                 mcpwm_timer_t pwm_timer,
                 mcpwm_io_signals_t io_signal_forward,
                 mcpwm_io_signals_t io_signal_backward,
                 mcpwm_config_t pwm_config,
                 float min_duty_cycle_percentage,
                 float max_duty_cycle_percentage):

                 axis(axis),
                 forward_pin(forward_pin),
                 backward_pin(backward_pin),
                 pwm_unit(pwm_unit),
                 pwm_timer(pwm_timer),
                 io_signal_forward(io_signal_forward),
                 io_signal_backward(io_signal_backward),
                 pwm_operator(mcpwm_operator_t (NULL)),
                 pwm_config(pwm_config),
                 min_duty_cycle_percentage(min_duty_cycle_percentage),
                 max_duty_cycle_percentage(max_duty_cycle_percentage),
                 direction(direction_t::BOTH),
                 pwm_num(2),
                 log("Pwm") {
}
}}