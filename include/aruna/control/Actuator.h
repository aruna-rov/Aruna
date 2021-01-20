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
            static double convert_range(uint16_t input, float range_max = 100.f, float range_min = 0.f);

        private:
            axis_mask_t axis = axis_mask_t::NONE;
            int16_t speed = 0;

            /**
             * Implementation of axis movement, this function is called from `set(...)`.
             * Only gets called when axisMask match axis of Actuator.
             * @param axisMask: axis (can be multiple) to set speed to.
             * @param speed: desired speed.
             * @return err_t::OK if successful, others when not...
             */
            virtual err_t _set(axis_mask_t axisMask, int16_t speed) = 0;


        public:
            /**
             * error when constructing gets put here, read before usage.
             */
            err_t startup_error = err_t::NOT_STARTED;

            /**
             * Actuator object, used by the control module for vehicle movement.
             * Define possible axis this Actuator can move in.
             * @param axis: axis_mask_t possible containing multiple axis
             */
//             TODO explicit?
            Actuator(axis_mask_t axis = axis_mask_t::NONE);



            /**
             * Set the axis that this Actuator is capable in moving in
             * @param new_axis: axis_mask_t possible containing multible axis
             * @return always err_t::OK
             */
            err_t set_axis(axis_mask_t new_axis);

            /**
            * get the control modes that this driver supports.
            * @return control_mode mask ORed.
            */
            axis_mask_t get_axis();

            /**
            * Set the speed of the motors directly
            * @param axisMask, multiple axis to apply speed to.
            * @param speed, speed of the motors
            * @return err_t::OK if the command was succesfull, others when it fails.
            */
            err_t set(axis_mask_t axisMask, int16_t speed);

            /**
             * Get the current speed of Actuator
             * @return speed.
             */
            uint16_t get_speed();





        };
    }
}

#endif //ARUNA_CONTROLACTUATOR_H
