//
// Created by noeel on 27-3-19.
//

#ifndef ARUNA_CONTROLACTUATORDRIVER_H
#define ARUNA_CONTROLACTUATORDRIVER_H

#include "aruna/arunaTypes.h"
#include "aruna/control/controlTypes.h"

namespace aruna { namespace control {

class ControlActuatorDriver {
public:
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
		return err_t::OK;
	};

	/**
	 * get the control modes that this driver supports.
	 * @return control_mode mask ORed.
	 */
	virtual axis_mask_t get_axis() = 0;

	/**
	 * Set the speed of the motors directly
	 * @param axisMask, multiple axis to apply speed to.
	 * @param speed, speed of the motors
	 * @param direction, direction to go to.
	 * @return err_t::OK if the command was succesfull, others when it fails.
	 */
	virtual err_t set(axis_mask_t axisMask, uint16_t speed, direction_t direction) = 0;

	/**
	 * Convert uint16 to a new range
	 * @param input input to convert
	 * @param range_max maximum range (default is 100)
	 * @return output of convertion
	 */
	static constexpr double convert_range(uint16_t input, float range_max=100.f) {
		return (range_max/65535) *input;
	}

};
}}

#endif //ARUNA_CONTROLACTUATORDRIVER_H
