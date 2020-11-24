//
// Created by noeel on 30-07-19.
//

#include "aruna/arunaTypes.h"
#include "aruna/control/ActuatorSet.h"
#include <math.h>

using namespace aruna::control;
namespace aruna {
	namespace control {
        log::channel_t ActuatorSet::log("ActuatorSet");
		ActuatorSet::ActuatorSet(transform_t *transform, size_t transform_size) : Actuator(axis_mask_t::NONE), transform(transform),
																				  transform_size(transform_size){

		    set_axis(compute_axis());
            startup_error = err_check();
		}

		err_t ActuatorSet::_set(axis_mask_t axisMask, int16_t speed) {
			err_t err = err_t::OK;
			err_t last_failure = err;
			for (int i = 0; i < transform_size; ++i) {
				if ((uint8_t) transform[i].transform_to & (uint8_t) axisMask) {
					int32_t adjusted_speed = (uint16_t) ((float) speed *
														  (transform[i].speed_percentage / (float) 100.0));
					adjusted_speed = transform[i].flip_direction ? adjusted_speed * -1: adjusted_speed;
					adjusted_speed = fmax(adjusted_speed, -32767);
					adjusted_speed = fmin(adjusted_speed, 32767);
					log.verbose("setting driver transformed: %i, axis: 0x%X, speed:%i", i, transform[i].axis,
								adjusted_speed);
					err = transform[i].driver->set(transform[i].axis, (uint16_t) adjusted_speed);
				} else {
					log.verbose("setting driver: %i, axis: 0x%X, speed:%i", i, axisMask, speed);
					err = transform[i].driver->set(axisMask, speed);
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
