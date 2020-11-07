//
// Created by noeel on 22-10-20.
//

#include <aruna/control/Actuator.h>

using namespace aruna;
using namespace aruna::control;

Actuator::~Actuator() {
    set(get_axis(), 0);
}

Actuator::Actuator(axis_mask_t axis) : axis(axis){

}

axis_mask_t Actuator::get_axis() {
    return axis;
}

err_t Actuator::set(axis_mask_t axisMask, int16_t speed) {
//    TODO check direction via axisMask with(uint8_t) axisMask & (uint8_t) axis_mask_t::DIRECTION_BOTH & (uint8_t) axis
//    or check direction with the speed value, of just leave direction check to implementation.
    if ((uint8_t) axisMask & (uint8_t) axis_mask_t::ALL_AXIS & (uint8_t) axis) {
        this->speed = speed;
        return _set(axisMask, speed);
    }
//    TODO return not avaliable error?
    return err_t::OK;
}

uint16_t Actuator::get_speed() {
    return speed;
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
