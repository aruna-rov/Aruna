//
// Created by noeel on 30-07-19.
//

#include "aruna/arunaTypes.h"
#include "aruna/control/ActuatorSet.h"
#include <algorithm>

using namespace aruna::control;
namespace aruna {
	namespace control {
        log::channel_t ActuatorSet::log("ActuatorSet");
		ActuatorSet::ActuatorSet(transform_t *transform, size_t transform_size) : Actuator(axis_mask_t::NONE, direction_t::NONE), transform(transform),
																				  transform_size(transform_size){

		    set_axis(compute_axis());
		    set_direction(compute_direction());
            startup_error = err_check();
		}

		err_t ActuatorSet::_set(axis_mask_t axisMask, uint16_t speed,
							   direction_t direction) {
			err_t err = err_t::OK;
			err_t last_failure = err;
			for (int i = 0; i < transform_size; ++i) {
				if ((uint8_t) transform[i].transform_to & (uint8_t) axisMask) {
					direction_t dir = (direction_t) ((transform[i].flip_direction == !(bool) direction));
					uint32_t adjusted_speed = (uint16_t) ((float) speed *
														  (transform[i].speed_percentage / (float) 100.0));
					adjusted_speed = fmax(adjusted_speed, 0x0);
					adjusted_speed = fmin(adjusted_speed, 0xFFFF);
					log.verbose("setting driver transformed: %i, axis: 0x%X, speed:%i, dir:%i", i, transform[i].axis,
								adjusted_speed, dir);
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

		axis_mask_t ActuatorSet::compute_axis() {
			axis_mask_t axis = axis_mask_t::NONE;
			for (int i = 0; i < transform_size; ++i) {
				axis = (axis_mask_t) ((uint8_t) axis | (uint8_t) transform[i].driver->get_axis() |
									  (uint8_t) transform[i].transform_to);
			}
			return axis;
		}

		direction_t ActuatorSet::compute_direction() {
			direction_t driver_dir = direction_t::NONE;
			bool plus = false;
            bool min = false;
            bool both = false;

			for (int i = 0; i < transform_size; ++i) {
			    driver_dir = transform[i].driver->get_direction();
			    plus = driver_dir == direction_t::PLUS || plus;
			    min = driver_dir == direction_t::MIN || min;
			    both = driver_dir == direction_t::BOTH || both;
			}
            if (plus && !min && !both)
                return direction_t::PLUS;
            if (min && !plus && !both)
                return direction_t::MIN;
            if (both)
                return direction_t::BOTH;
            return direction_t::NONE;
		}

        err_t ActuatorSet::err_check() {
            err_t err = err_t::OK;
            err_t last_failure = err;
            for (int i = 0; i < transform_size; ++i) {
                err = transform[i].driver->startup_error;
                if ((uint8_t) err) {
                    last_failure = err;
                }
            }
            return last_failure;
        }

    }
}