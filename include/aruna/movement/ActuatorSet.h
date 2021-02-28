//
// Created by noeel on 30-07-19.
//

#ifndef ARUNA_ACTUATORSET_H
#define ARUNA_ACTUATORSET_H

#include <aruna.h>
#include "Actuator.h"

namespace aruna {
	namespace movement {
		class ActuatorSet : public Actuator {
		private:
            err_t _set(axis_mask_t axisMask, int16_t speed) override;
        public:
			struct transform_t {
				/**
				 * pointer to driver
				 */
				Actuator *driver;

				/**
				 * axis this driver can transform to
				 */
				axis_mask_t transform_to;

				/**
				 * flip direction on transform?
				 */
				bool flip_direction;

				/**
				 * axis to be set on driver to accomplish transform
				 */
				axis_mask_t axis;

				/**
				 * speed percentage
				 */
				float speed_percentage;
//					TODO make speed_percentage optional
			};


			/**
			 * put diffrent drivers in a driver set to allow them to cooperate and transform into a different axis when turned on together
			 * @param transform array of transform_t
			 * @param transform_size size of array
			 */
			ActuatorSet(transform_t *transform, size_t transform_size);


		private:
			transform_t *transform;
			const size_t transform_size;
			static aruna::log::channel_t log;

			/**
			 * Get alle the axis from all inside the set and put them together.
			 * @return axis mask of all actuators in the set.
			 */
            axis_mask_t compute_axis();


            err_t err_check();
		};
	}
}
#endif //ARUNA_ACTUATORSET_H
