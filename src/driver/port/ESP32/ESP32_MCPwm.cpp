//
// Created by noeel on 07-11-20.
//

#include "aruna/driver/port/ESP32/ESP32_MCPwm.h"

using namespace aruna;
using namespace aruna::driver;


ESP32_MCPwm::ESP32_MCPwm(gpio_num_t pin,
                         mcpwm_unit_t pwm_unit,
                         mcpwm_timer_t pwm_timer,
                         mcpwm_io_signals_t io_signal,
                         mcpwm_operator_t pwm_operator,
                         mcpwm_config_t pwm_config) :
        pin(pin),
        pwm_unit(pwm_unit),
        pwm_timer(pwm_timer),
        io_signal(io_signal),
        pwm_operator(pwm_operator),
        pwm_config(pwm_config) {
    ESP_ERROR_CHECK(mcpwm_gpio_init(pwm_unit, io_signal, pin));

//  TODO what happens if mcpwm_init runs two times?
    ESP_ERROR_CHECK(mcpwm_init(pwm_unit, pwm_timer, &pwm_config));
}

err_t ESP32_MCPwm::_set_duty(uint16_t duty) {
    float per_up = convert_range(duty);

    ESP_ERROR_CHECK(mcpwm_set_duty(pwm_unit, pwm_timer, pwm_operator, per_up));

    ESP_ERROR_CHECK(mcpwm_set_duty_type(pwm_unit, pwm_timer, pwm_operator, MCPWM_DUTY_MODE_0));
    return err_t::OK;
}

err_t ESP32_MCPwm::_set_frequency(uint32_t frequency_hz) {
    ESP_ERROR_CHECK(mcpwm_set_frequency(pwm_unit, pwm_timer, frequency_hz));
//    TODO return esp error.
//    TODO override get_frequency function.
    return err_t::OK;
}
