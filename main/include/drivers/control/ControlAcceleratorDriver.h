//
// Created by noeel on 27-3-19.
//

#ifndef ARUNA_CONTROLACCELERATORDRIVER_H
#define ARUNA_CONTROLACCELERATORDRIVER_H


#include "drivers/control.h"
#include "../control.h"

class ControlAcceleratorDriver {
public:
	/**
	 * start the driver
	 */
	virtual control_err_t start() {
		return CONTROL_OK;
	};

	/**
	 * stop the driver
	 */
	virtual control_err_t stop() {
		return CONTROL_OK;
	};

	/**
	 * get the control modes that this driver supports.
	 * @return control_mode mask ORed.
	 */
	virtual control_axis_mask_t get_axis() = 0;

//	TODO documentation
	virtual	void set(control_axis_mask_t axisMask, uint16_t speed, control_direction_t direction) = 0;

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


#endif //ARUNA_CONTROLACCELERATORDRIVER_H
