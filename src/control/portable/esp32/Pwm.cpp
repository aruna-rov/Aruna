//
// Created by noeel on 30-3-19.
//

#include <driver/mcpwm.h>
#include "aruna/control/portable/esp32/Pwm.h"
#include "aruna/control.h"
namespace aruna { namespace drivers { namespace control {
using namespace aruna::control;

axis_mask_t Pwm::get_axis() {
    return axis;
}

err_t Pwm::start() {
    ESP_ERROR_CHECK(mcpwm_gpio_init(pwm_unit, io_signal_forward, forward_pin));
	ESP_ERROR_CHECK(mcpwm_gpio_init(pwm_unit, io_signal_backward, backward_pin));


    mcpwm_config_t pwm_config = {
            100,
            0,
            0,
            MCPWM_DUTY_MODE_0,
            MCPWM_UP_COUNTER,
    };

    ESP_ERROR_CHECK(mcpwm_init(pwm_unit, pwm_timer, &pwm_config));
    return err_t::OK;
}

err_t Pwm::stop() {
	set(axis_mask_t::ALL, 0, direction_t::PLUS);
    return err_t::OK;
}

err_t Pwm::set(axis_mask_t axisMask, uint16_t speed, direction_t direction) {
	log.debug("Pwm", "axis:%X, speed:%d, dir:%d", (int) axisMask, speed, (int) direction);
	if ((uint8_t) axisMask & (uint8_t) axis) {
		float per_up = convert_range(speed);
		mcpwm_operator_t low = (bool) direction ? MCPWM_OPR_A : MCPWM_OPR_B;
		mcpwm_operator_t high = (bool) direction ? MCPWM_OPR_B : MCPWM_OPR_A;

		ESP_ERROR_CHECK(mcpwm_set_signal_low(pwm_unit, pwm_timer, low));

		ESP_ERROR_CHECK(mcpwm_set_duty(pwm_unit, pwm_timer, high, per_up));

		ESP_ERROR_CHECK(mcpwm_set_duty_type(pwm_unit, pwm_timer, high, MCPWM_DUTY_MODE_0));
	}
	return err_t::OK;
}

Pwm::Pwm(aruna::control::axis_mask_t axis, gpio_num_t forward_pin, gpio_num_t backward_pin,
		 mcpwm_unit_t pwm_unit, mcpwm_timer_t pwm_timer, mcpwm_io_signals_t io_signal_forward, mcpwm_io_signals_t io_signal_backward)
			 : axis(axis), forward_pin(forward_pin), backward_pin(backward_pin), pwm_unit(pwm_unit), pwm_timer(pwm_timer), io_signal_forward(io_signal_forward), io_signal_backward(io_signal_backward), log("Pwm") {
//	TODO allow for single pwm pin
}
}}}