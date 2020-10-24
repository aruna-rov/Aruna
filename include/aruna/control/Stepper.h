//
// Created by noeel on 04-02-20.
//

#ifndef ARUNA_STEPPER_H
#define ARUNA_STEPPER_H

#include <stdint.h>
#include "Actuator.h"
#include "aruna/log.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <freertos/semphr.h>

namespace aruna {
    namespace control {

        class Stepper : public Actuator {
        private:
            SemaphoreHandle_t asked_set_mutex = NULL;
            uint16_t asked_speed;
            direction_t asked_direction;
            size_t active_pin_index = 0;
            TaskHandle_t timer_task_handle;
            const axis_mask_t axis;
            const direction_t direction;
            const uint8_t* pins;
            const size_t pins_count = 0;
            const bool active_high;
            log::channel_t *log;

            /**
             * initialize the pins for output
             * @return err_t
             */
            err_t init_pins();

            /**
             * reset the pins back to initial state (run when program is finished)
             * @return err_t
             */
            err_t clear_pins();

            /**
             * init single pin as output
             * @param pin_nr, pin to initialize
             * @return err_t
             */
            virtual err_t init_pin(uint8_t pin_nr) = 0;

            /**
             * set pin level high or low
             * @param pin_nr, pin to set
             * @param value, bool. true for high, false for low
             * @return err_t
             */
            virtual err_t set_pin(uint8_t pin_nr, bool value) = 0;

            /**
             * clear single pin after program is finished
             * @param pin_nr pin to clear
             * @return err_t
             */
            virtual err_t clear_pin(uint8_t pin_nr) {
                return err_t::OK;
            }

            /**
             * Do a single step in a direction
             * @param direction PLUS or MINUS
             * @return err_t
             */
            err_t do_step(direction_t direction);

            /**
             * pop pin from circular array buffer.
             * @param direction, direction to pop
             * @param n, offset of current active pin, default 0
             * @return pin_nr
             */
            uint8_t get_pin(direction_t direction, uint8_t n = 0);

            /**
             * task to set the next phase on time.
             */
            void timer_task();
            /**
             * wrapper for timer_task(), as FreeRTOS only wants to create tasks from static functions
             * @param _this current object to timer_task() can be called
             */
            static void _timer_task_wrapper(void* _this);

            err_t _set(axis_mask_t axisMask, uint16_t speed, direction_t direction) override;

        public:
            /**
             * Stepper motor
             * @param pins, pointer GPIO number array, used in order for phase shifting
             * @param pins_count, number of entries in `pins`
             * @param axis, axis mask to work on
             * @param direction, accepted directions
             * @param active_high, should the current phase be high and other low or current phase low and other pins high
             */
            Stepper(uint8_t *pins, size_t pins_count, axis_mask_t axis, direction_t direction, bool active_high);

            /**
             * destructor, call `stop()` before though.
             */
            ~Stepper();



            };
    }
}
#endif //ARUNA_STEPPER_H
