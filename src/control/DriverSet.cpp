//
// Created by noeel on 30-07-19.
//

#include "aruna/arunaTypes.h"
#include "aruna/control/DriverSet.h"
#include <algorithm>

using namespace aruna::control;
namespace aruna {
	namespace drivers {
		namespace control {
			DriverSet::DriverSet(transform_t *transform, size_t transform_size) : transform(transform),
																				  transform_size(transform_size),
																				  log("DriverSet"){}
//																				  TODO what happends to 'log' if multiple DriverSet's are made?

			err_t DriverSet::set(aruna::control::axis_mask_t axisMask, uint16_t speed,
												 aruna::control::direction_t direction) {
				err_t err = err_t::OK;
				err_t last_failure = err;
				for (int i = 0; i < transform_size; ++i) {
					if ((uint8_t) transform[i].transform_to & (uint8_t) axisMask) {
						direction_t dir = (direction_t) ((transform[i].flip_direction == !(bool) direction));
						uint32_t adjusted_speed = (uint16_t) ((float) speed *
															  (transform[i].speed_percentage / (float) 100.0));
						adjusted_speed = fmax(adjusted_speed, 0x0);
						adjusted_speed = fmin(adjusted_speed, 0xFFFF);
						log.verbose("setting driver transformed: %i, axis: 0x%X, speed:%i, dir:%i", i, transform[i].axis, adjusted_speed, dir);
						err = transform[i].driver->set(transform[i].axis, (uint16_t) adjusted_speed, dir);
					} else {
						log.verbose("setting driver: %i, axis: 0x%X, speed:%i, dir:%i", i, axisMask, speed, direction);
						err = transform[i].driver->set(axisMask, speed, direction);
					}
					if ((uint8_t) err) {
						last_failure = err;
					}
				}
				return last_failure;
			}

			aruna::control::axis_mask_t DriverSet::get_axis() {
				aruna::control::axis_mask_t axis = axis_mask_t::NONE;
				for (int i = 0; i < transform_size; ++i) {
					axis = (axis_mask_t) ((uint8_t) axis | (uint8_t) transform[i].driver->get_axis() |
										  (uint8_t) transform[i].transform_to);
				}
				return axis;
			}

			err_t DriverSet::stop() {
				err_t err = err_t::OK;
				err_t last_failure = err;
				for (int i = 0; i < transform_size; ++i) {
					err = transform[i].driver->stop();
					if ((uint8_t) err) {
						last_failure = err;
					}
				}
				return last_failure;
			}

			err_t DriverSet::start() {
				err_t err = err_t::OK;
				err_t last_failure = err;
				for (int i = 0; i < transform_size; ++i) {
					err = transform[i].driver->start();
					if ((uint8_t) err) {
						last_failure = err;
					}
				}
				return last_failure;
			}
		}
	}
}