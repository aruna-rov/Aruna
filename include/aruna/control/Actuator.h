//
// Created by noeel on 27-3-19.
//

#ifndef ARUNA_CONTROLACTUATOR_H
#define ARUNA_CONTROLACTUATOR_H

#include "aruna/arunaTypes.h"
#include "aruna/control/controlTypes.h"

namespace aruna { namespace control {

class Actuator {
public:
//    TODO direction and axis variable should be declared here
	/**
	 * start the driver
	 */
	virtual err_t start() {
		return err_t::OK;
	};

	/**
	 * stop the driver
	 */
	virtual err_t stop() {
//	    TODO destructor and constructor instead of start() and stop()
        set(axis_mask_t::ALL, 0, direction_t::PLUS);
        return err_t::OK;
	};

	/**
	 * get the control modes that this driver supports.
	 * @return control_mode mask ORed.
	 */
//	 TODO return default axis variables
	virtual axis_mask_t get_axis() = 0;

	/**
	 * Set the speed of the motors directly
	 * @param axisMask, multiple axis to apply speed to.
	 * @param speed, speed of the motors
	 * @param direction, direction to go to.
	 * @return err_t::OK if the command was succesfull, others when it fails.
	 */
//	 TODO axis and direction check is always the same, make a standard default one.
	virtual err_t set(axis_mask_t axisMask, uint16_t speed, direction_t direction) = 0;

	/**
	 * Convert uint16 to a new range
	 * @param input input to convert
	 * @param range_max maximum range (default is 100)
	 * @param range_min minimum range (default is 0)
	 * @return output of convertion
	 */
	static constexpr double convert_range(uint16_t input, float range_max=100.f, float range_min=0.f) {
//	    TODO allow for other type then uint_16 (maybe use <template>? and sizeof()?)
        return ((input * (range_max - range_min)) / 65535) + range_min;
	}

};
}}

#endif //ARUNA_CONTROLACTUATOR_H
