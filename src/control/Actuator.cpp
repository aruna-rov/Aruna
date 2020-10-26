//
// Created by noeel on 22-10-20.
//

#include <aruna/control/Actuator.h>

using namespace aruna;
using namespace aruna::control;

Actuator::~Actuator() {
    set(get_axis(), 0, get_direction());
}

Actuator::Actuator(axis_mask_t axis, direction_t direction) : axis(axis), direction(direction) {

}

axis_mask_t Actuator::get_axis() {
    return axis;
}

err_t Actuator::set(axis_mask_t axisMask, uint16_t speed, direction_t direction) {
    if ((uint8_t) axisMask & (uint8_t) axis && (this->direction == direction_t::BOTH || this->direction == direction)) {
        this->speed = speed;
        return _set(axisMask, speed, direction);
    }
//    TODO return not avaliable error?
    return err_t::OK;
}

uint16_t Actuator::get_speed() {
    return speed;
}

direction_t Actuator::get_direction() {
    return direction;
}

err_t Actuator::set_direction(direction_t new_direction) {
    direction = new_direction;
    return err_t::OK;
}

err_t Actuator::set_axis(axis_mask_t new_axis) {
    axis = new_axis;
    return err_t::OK;
}

// TODO constexpr?
double Actuator::convert_range(uint16_t input, float range_max, float range_min) {
//	    TODO allow for other type then uint_16 (maybe use <template>? and sizeof()?)
    return ((input * (range_max - range_min)) / 65535) + range_min;
}
