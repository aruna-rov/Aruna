//
// Created by noeel on 27-3-19.
//

#ifndef ARUNA_CONTROLACTUATOR_H
#define ARUNA_CONTROLACTUATOR_H

#include "aruna/arunaTypes.h"
#include "aruna/control/controlTypes.h"

namespace aruna {
    namespace control {

        class Actuator {
        protected:


            /**
             * Convert uint16 to a new range
             * @param input input to convert
             * @param range_max maximum range (default is 100)
             * @param range_min minimum range (default is 0)
             * @return output of convertion
             */
            static constexpr double convert_range(uint16_t input, float range_max = 100.f, float range_min = 0.f) {
//	    TODO allow for other type then uint_16 (maybe use <template>? and sizeof()?)
                return ((input * (range_max - range_min)) / 65535) + range_min;
            }

            err_t set_direction(direction_t new_direction);

            err_t set_axis(axis_mask_t new_axis);

        private:
            axis_mask_t axis = axis_mask_t::NONE;
            direction_t direction = direction_t::NONE;
            uint16_t speed = 0;

            virtual err_t _set(axis_mask_t axisMask, uint16_t speed, direction_t direction) = 0;


        public:
            err_t startup_error = err_t::NOT_STARTED;

            Actuator(axis_mask_t axis, direction_t direction);

            virtual ~Actuator();

            /**
            * get the control modes that this driver supports.
            * @return control_mode mask ORed.
            */
            axis_mask_t get_axis();

            /**
            * Set the speed of the motors directly
            * @param axisMask, multiple axis to apply speed to.
            * @param speed, speed of the motors
            * @param direction, direction to go to.
            * @return err_t::OK if the command was succesfull, others when it fails.
            */
            err_t set(axis_mask_t axisMask, uint16_t speed, direction_t direction);

            /**
             * Get the current speed of Actuator
             * @return speed.
             */
            uint16_t get_speed();

            /**
             * Get the movement direction
             * @return direction.
             */
            direction_t get_direction();




        };
    }
}

#endif //ARUNA_CONTROLACTUATOR_H
