//
// Created by noeel on 30-3-19.
//

#include <driver/mcpwm.h>
#include <esp_log.h>
#include "aruna/drivers/control/L293D.h"
#include "aruna/control.h"
namespace aruna { namespace drivers { namespace control {
using namespace aruna::control;
namespace {
    const gpio_num_t PIN_A[2] = {GPIO_NUM_13, GPIO_NUM_12};
    const gpio_num_t PIN_B[2] = {GPIO_NUM_14, GPIO_NUM_27};
    const mcpwm_unit_t PWM_UNIT_A = MCPWM_UNIT_0;
    const mcpwm_unit_t PWM_UNIT_B = MCPWM_UNIT_1;
    const mcpwm_timer_t PWM_TIMER = MCPWM_TIMER_0;
}

axis_mask_t L293D::get_axis() {
    return (axis_mask_t) ( (uint8_t) axis_mask_t::X | (uint8_t) axis_mask_t::YAW);
}

err_t L293D::start() {
//	A
    ESP_ERROR_CHECK(mcpwm_gpio_init(PWM_UNIT_A, MCPWM0A, PIN_A[0]));
	ESP_ERROR_CHECK(mcpwm_gpio_init(PWM_UNIT_A, MCPWM0B, PIN_A[1]));
//    B
	ESP_ERROR_CHECK(mcpwm_gpio_init(PWM_UNIT_B, MCPWM0A, PIN_B[0]));
	ESP_ERROR_CHECK(mcpwm_gpio_init(PWM_UNIT_B, MCPWM0B, PIN_B[1]));

    mcpwm_config_t pwm_config = {
            100,
            0,
            0,
            MCPWM_DUTY_MODE_0,
            MCPWM_UP_COUNTER,
    };

    ESP_ERROR_CHECK(mcpwm_init(PWM_UNIT_A, PWM_TIMER, &pwm_config));
    ESP_ERROR_CHECK(mcpwm_init(PWM_UNIT_B, PWM_TIMER, &pwm_config));
    return err_t::OK;
}

err_t L293D::stop() {
	set(axis_mask_t::ALL, 0, direction_t::PLUS);
    return err_t::OK;
}

err_t L293D::set(axis_mask_t axisMask, uint16_t speed, direction_t direction) {
	ESP_LOGD("L293D", "axis:%X, speed:%d, dir:%d", (int) axisMask, speed, (int) direction);
	if ((uint8_t) axisMask & (uint8_t) axis_mask_t::X) {
		float per_up = convert_range(speed);
		mcpwm_operator_t low = (bool) direction ? MCPWM_OPR_A : MCPWM_OPR_B;
		mcpwm_operator_t high = (bool) direction ? MCPWM_OPR_B : MCPWM_OPR_A;

		ESP_ERROR_CHECK(mcpwm_set_signal_low(PWM_UNIT_A, PWM_TIMER, low));
		ESP_ERROR_CHECK(mcpwm_set_signal_low(PWM_UNIT_B, PWM_TIMER, low));

		ESP_ERROR_CHECK(mcpwm_set_duty(PWM_UNIT_A, PWM_TIMER, high, per_up));
		ESP_ERROR_CHECK(mcpwm_set_duty(PWM_UNIT_B, PWM_TIMER, high, per_up));

		ESP_ERROR_CHECK(mcpwm_set_duty_type(PWM_UNIT_A, PWM_TIMER, high, MCPWM_DUTY_MODE_0));
		ESP_ERROR_CHECK(mcpwm_set_duty_type(PWM_UNIT_B, PWM_TIMER, high, MCPWM_DUTY_MODE_0));
	}
	else if ((uint8_t) axisMask & (uint8_t) axis_mask_t::YAW) {
		float per_up = convert_range(speed);
		mcpwm_operator_t low = (bool) direction ? MCPWM_OPR_A : MCPWM_OPR_B;
		mcpwm_operator_t high = (bool) direction ? MCPWM_OPR_B : MCPWM_OPR_A;

		ESP_ERROR_CHECK(mcpwm_set_signal_low(PWM_UNIT_A, PWM_TIMER, low));
		ESP_ERROR_CHECK(mcpwm_set_signal_low(PWM_UNIT_B, PWM_TIMER, high));

		ESP_ERROR_CHECK(mcpwm_set_duty(PWM_UNIT_A, PWM_TIMER, high, per_up));
		ESP_ERROR_CHECK(mcpwm_set_duty(PWM_UNIT_B, PWM_TIMER, low, per_up));

		ESP_ERROR_CHECK(mcpwm_set_duty_type(PWM_UNIT_A, PWM_TIMER, high, MCPWM_DUTY_MODE_0));
		ESP_ERROR_CHECK(mcpwm_set_duty_type(PWM_UNIT_B, PWM_TIMER, low, MCPWM_DUTY_MODE_0));
	}
	return err_t::OK;
}
}}}