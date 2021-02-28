//
// Created by noeel on 18-10-20.
//

#include <cmath>
#include "aruna/driver/Pwm.h"

using namespace aruna::driver;
using namespace aruna;

uint16_t Pwm::get_duty() const {
    return duty;
}

uint32_t Pwm::get_frequency() const {
    return frequency_hz;
}

float Pwm::get_duty_per() const {
    return duty / 65535 * 100;
}

err_t Pwm::set_duty(float duty_percentage) {
    uint16_t duty = round(duty_percentage / 100 * 65535);
    return set_duty(duty);
}

err_t Pwm::set_frequency(uint32_t frequency_hz) {
    err_t err = _set_frequency(frequency_hz);
    if (err == err_t::OK) {
        this->frequency_hz = frequency_hz;
    }
    return err;
}

err_t Pwm::set_duty(uint16_t duty) {
    err_t err = _set_duty(duty);
    if (err == err_t::OK) {
        this->duty = duty;
    }
    return err;
}

err_t Pwm::_set(movement::axis_mask_t axisMask, int16_t speed) {
//    TODO allow for min and max duty_cycle
//    TODO multi directional pwm
//    TODO startup_error is not set to OK. Set to standard to OK, remove all together or standalize.
    return set_duty((uint16_t)speed);
}
