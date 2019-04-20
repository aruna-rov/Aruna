//
// Created by noeel on 30-3-19.
//

#include <driver/mcpwm.h>
#include <esp_log.h>
#include "drivers/control/L293D.h"

namespace {
    const gpio_num_t PIN_A[2] = {GPIO_NUM_13, GPIO_NUM_12};
    const gpio_num_t PIN_B[2] = {GPIO_NUM_14, GPIO_NUM_27};
    const mcpwm_unit_t PWM_UNIT_A = MCPWM_UNIT_0;
    const mcpwm_unit_t PWM_UNIT_B = MCPWM_UNIT_1;
    const mcpwm_timer_t PWM_TIMER = MCPWM_TIMER_0;
}

control_axis_mask_t L293D::get_axis() {
    return CONTROL_AXIS_MASK_X;
}

control_err_t L293D::start() {
//	A
    mcpwm_gpio_init(PWM_UNIT_A, MCPWM0A, PIN_A[0]);
    mcpwm_gpio_init(PWM_UNIT_A, MCPWM0B, PIN_A[1]);
//    B
    mcpwm_gpio_init(PWM_UNIT_B, MCPWM0A, PIN_B[0]);
    mcpwm_gpio_init(PWM_UNIT_B, MCPWM0B, PIN_B[1]);

    mcpwm_config_t pwm_config = {
            100,
            0,
            0,
            MCPWM_DUTY_MODE_0,
            MCPWM_UP_COUNTER,
    };

    mcpwm_init(PWM_UNIT_A, PWM_TIMER, &pwm_config);
    mcpwm_init(PWM_UNIT_B, PWM_TIMER, &pwm_config);
    return ControlAcceleratorDriver::start();
}

control_err_t L293D::stop() {
	set(CONTROL_AXIS_MASK_ALL, 0, CONTROL_DIRECTION_PLUS);
    return ControlAcceleratorDriver::stop();
}

void L293D::set(control_axis_mask_t modes, uint16_t speed, control_direction_t direction) {
	ESP_LOGD("L293D", "mode:%X, speed:%d, dir:%d", modes, speed, direction);
	if (modes & CONTROL_AXIS_MASK_X) {
		float per_up = convert_range(speed);
		mcpwm_operator_t low = direction ? MCPWM_OPR_A : MCPWM_OPR_B;
		mcpwm_operator_t high = direction ? MCPWM_OPR_B : MCPWM_OPR_A ;

		mcpwm_set_signal_low(PWM_UNIT_A, PWM_TIMER, low);
		mcpwm_set_signal_low(PWM_UNIT_B, PWM_TIMER, low);

		mcpwm_set_duty(PWM_UNIT_A, PWM_TIMER, high, per_up);
		mcpwm_set_duty(PWM_UNIT_B, PWM_TIMER, high, per_up);

		mcpwm_set_duty_type(PWM_UNIT_A, PWM_TIMER, high, MCPWM_DUTY_MODE_0);
		mcpwm_set_duty_type(PWM_UNIT_B, PWM_TIMER, high, MCPWM_DUTY_MODE_0);
	}
}
