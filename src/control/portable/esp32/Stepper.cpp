//
// Created by noeel on 06-02-20.
//

#include "aruna/control/portable/esp32/Stepper.h"
#include <driver/gpio.h>

aruna::err_t aruna::control::esp32::Stepper::init_pin(uint8_t pin_nr) {
//    TODO use gpio_config() instead
    if (gpio_set_direction((gpio_num_t) pin_nr, GPIO_MODE_OUTPUT) != ESP_OK)
        return err_t::INVALID_PARAMETERS;
    else
        return err_t::OK;
}

aruna::err_t aruna::control::esp32::Stepper::set_pin(uint8_t pin_nr, bool value) {
    if (gpio_set_level((gpio_num_t)pin_nr, value) != ESP_OK)
        return err_t::INVALID_PARAMETERS;
    else
        return err_t::OK;
}

aruna::err_t aruna::control::esp32::Stepper::clear_pin(uint8_t pin_nr) {
    gpio_reset_pin((gpio_num_t)pin_nr);
    return err_t::OK;
}
