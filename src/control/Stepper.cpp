//
// Created by noeel on 04-02-20.
//

#include "aruna/control/Stepper.h"

aruna::err_t aruna::control::Stepper::set(aruna::control::axis_mask_t axisMask, uint16_t speed,
                                          aruna::control::direction_t direction) {
//    match axis
    if ((uint8_t) axisMask && (uint8_t) axis) {
//        TODO check if direction match
        if (xSemaphoreTake(asked_set_mutex, portMAX_DELAY) != pdTRUE)
            log->error("failed to take mutex");
        asked_speed = speed;
        asked_direction = direction;
        if (xSemaphoreGive(asked_set_mutex) != pdTRUE)
            log->error("failed to give mutex");
        xTaskNotifyGive(timer_task_handle);
    }
    return err_t::OK;
}

aruna::err_t aruna::control::Stepper::do_step(direction_t direction) {
// TODO half stepping
    err_t last_err_msg;
    for (int i = 0; i < pins_count ; ++i) {
        if (i == active_pin_index)
            continue;
        last_err_msg = set_pin(pins[i], !active_high);
    }
    last_err_msg = set_pin(get_pin(direction), active_high);

    return last_err_msg;
}

aruna::control::Stepper::Stepper(uint8_t *pins, size_t pins_count, axis_mask_t axis, direction_t direction,
                                 bool active_high)
        : axis(axis), direction(direction), pins(pins), pins_count(pins_count), active_high(active_high) {
//  TODO creating two stepper object causes error on registering log
//  TODO break_on_idle bool parameter. If true: lock motor when speed is zero; false: release motor is speed is zero.
    log = new log::channel_t("stepper");

    asked_set_mutex = xSemaphoreCreateMutex();
    if (asked_set_mutex == NULL) {
        log->error("failed to create mutex");
    }
    if (xTaskCreate(_timer_task_wrapper, "stepper timer", 2048, this, 10, &timer_task_handle) != pdTRUE)
        log->error("failed to create stepper timer task");

}

aruna::control::Stepper::~Stepper() {
// TODO save to call stop() from here?
// because stop() calls a pure virtual function.
}

uint8_t aruna::control::Stepper::get_pin(direction_t direction, uint8_t n) {
    uint8_t ret_val = pins[active_pin_index + n];
//        circular buffer
    if (n == 0 and direction == direction_t::PLUS) {
//        move to next pin
        active_pin_index = (active_pin_index + 1) >= pins_count ? 0 : active_pin_index + 1;
    } else if (n == 0 and direction == direction_t::MIN) {
//        move to prev pin
        active_pin_index = ((int32_t)(active_pin_index - 1)) < 0 ? pins_count - 1 : active_pin_index - 1;
    }
    return ret_val;
}

void aruna::control::Stepper::timer_task() {
    uint16_t speed = 0;
    direction_t direction = direction_t::PLUS;
    uint16_t delay_ms = 0;
    bool first_time = true;
    err_t err_msg;
    while (true) {

        if (speed == 0)
//            wait indefenitly if speed is zero
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (ulTaskNotifyTake(pdTRUE, 0) or first_time) {
            first_time = false;
//        update our variables
            if (xSemaphoreTake(asked_set_mutex, portMAX_DELAY) != pdTRUE) {
                log->error("failed to take mutex");
            }
            speed = asked_speed;
            direction = asked_direction;
            if (xSemaphoreGive(asked_set_mutex) != pdTRUE) {
                log->error("failed to give mutex");
            }
//            TODO refine delay to more reasonable speeds
            delay_ms = 65536 - speed;

        }
//        loop the pins!
        err_msg = do_step(direction);
        if(err_msg != err_t::OK)
            log->error("failed to step: %s", err_to_char.at(err_msg));
        vTaskDelay(delay_ms / portTICK_PERIOD_MS);

    }

}

void aruna::control::Stepper::_timer_task_wrapper(void *_this) {
    (static_cast<Stepper *>(_this))->timer_task();
}

aruna::control::axis_mask_t aruna::control::Stepper::get_axis() {
    return axis;
}

aruna::err_t aruna::control::Stepper::init_pins() {
    err_t msg = err_t::OK;
    for (int i = 0; i < pins_count; ++i) {
        msg = init_pin(pins[i]);
        if (msg != err_t::OK) {
            log->error("failed to init pin %i: %s", pins[i], err_to_char.at(msg));
        }
    }
    return msg;
}

aruna::err_t aruna::control::Stepper::clear_pins() {
    err_t msg = err_t::OK;
    for (int i = 0; i < pins_count; ++i) {
        msg = clear_pin(pins[i]);
        if (msg != err_t::OK) {
            log->error("failed to close pin %i: %s", pins[i], err_to_char.at(msg));
        }
    }
    return msg;
}

aruna::err_t aruna::control::Stepper::start() {
    init_pins();
    return Actuator::start();
}

aruna::err_t aruna::control::Stepper::stop() {
    //                close the pins
    clear_pins();
    vTaskDelete(timer_task_handle);
    return Actuator::stop();
}
