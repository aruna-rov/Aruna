//
// Created by noeel on 30-07-19.
//

#ifndef ARUNA_DRIVERSET_H
#define ARUNA_DRIVERSET_H

#include <aruna.h>
#include "ControlActuatorDriver.h"

namespace aruna {
	namespace control {
		class DriverSet : public ControlActuatorDriver {
		public:
			struct transform_t {
				/**
				 * pointer to driver
				 */
				ControlActuatorDriver *driver;

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

			axis_mask_t get_axis() override;

			err_t
			set(axis_mask_t axisMask, uint16_t speed, direction_t direction) override;

			/**
			 * put diffrent drivers in a driver set to allow them to cooperate and transform into a different axis when turned on together
			 * @param transform array of transform_t
			 * @param transform_size size of array
			 */
			DriverSet(transform_t *transform, size_t transform_size);

			err_t stop() override;

			err_t start() override;

		private:
			transform_t *transform;
			const size_t transform_size;
			aruna::log::channel_t log;
		};
	}
}
#endif //ARUNA_DRIVERSET_H
