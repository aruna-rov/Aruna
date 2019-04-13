//
// Created by noeel on 6-1-19.
//

#ifndef ARUNA_CONTROL_H
#define ARUNA_CONTROL_H

#include <stdint.h>

class ControlAcceleratorDriver;

typedef enum {
    CONTROL_X = (1 << 0),
    CONTROL_Y = (1 << 1),
    CONTROL_Z = (1 << 2),
    CONTROL_ROLL = (1 << 3),
    CONTROL_YAW = (1 << 4),
    CONTROL_PITCH = (1 << 5),

    CONTROL_AXIS_MAX = 6,

} control_axis_mask_t;

typedef enum {
    CONTROL_OK,
    CONTROL_FAIL,
    CONTROL_ERR_NO_HARDWARE_FOUND,
    CONTROL_ERR_NO_DRIVER_FOUND,
    CONTROL_ERR_DRIVER_EXISTS,
    CONTROL_ERR_NO_ACTIVE_MODULES,
    CONTROL_ERR_NOT_STARTED,
    CONTROL_ERR_ALREADY_STARTED,
    CONTROL_ERR_MODE_NOT_ACTIVE,
    CONTROL_ERR_HARDWARE_FAILURE,
    CONTROL_ERR_OVERFLOW,
    CONTROL_ERR_UNDERFLOW
} control_err_t;

typedef enum {
    DAMP_ALLOW = 0,
    DAMP_STAY_AT_POSITION = 1,
    DAMP_STAND_STILL = 2
} control_damping_t;

// status of control
typedef enum  {
    CONTROL_RUNNING = 0,
    CONTROL_STOPPED = 1,
} control_status_t;

typedef enum {
    CONTROL_DIRECTION_PLUS = 0,
    CONTROL_DIRECTION_MIN = 1
} control_direction_t;

// variables


// functions


/**
 * Get active modus (X,Y,Z,yawn,pitch,roll)
 * @return `control_mode_t` with a bit high on enabled modus. Use `control_mode_mask_t` to decipher.
 */
control_axis_mask_t control_get_active_axis();

/**
 * initialise control and communicate with hardware for active modes.
 * @return control_err_t
 *  * `CONTROL_OK` started successfully,
 *  * `CONTROL_ERR_NO_HARDWARE_FOUND` could not find andy drivers,
 *  * `CONTROL_ERR_NO_ACTIVE_MODULES` no module reported active,
 *  * `CONTROL_ERR_ALREADY_STARTED` control is already active,
 */
control_status_t control_start();

/**
 * Stop all motors and free all processes.
 * @return control_err_t
 *  * `CONTROL_OK` stopped successfully.
 *  * `CONTROL_ERR_NOT_STARTED` control was not started yet.
 */
control_status_t control_stop();

/**
 * get the status of the control unit.
 * @return status enum
 */
control_status_t control_get_status();

/**
 * FreeRTOS task to handle all the incoming com requests.
 * @param arg, not used.
 */
void control_com_handler_task(void* arg);

/**
 * damping task to keep the device at a current speed, altitude or position.
 * @param arg, not used.
 */
void control_damping_task(void* arg);

/**
 * set the damping type.
 * * `DAMP_ALLOW` to allow damping.
 * * `DAMP_STAY_AT_POSITION` stay at current position and return if moved,
 * * `DAMP_STAND_STILL` dont move
 * @param damp control_damping_t, mode to set damping to.
 */
// TODO documentatie bijwerken
void control_set_damping(control_axis_mask_t axisMask, control_damping_t damp);

/**
 * get the damping of a single mode.
 * @param single_axis only 1 bit to high.
 * @return damping of that mode.
 */
control_damping_t control_get_damping(control_axis_mask_t single_axis);
/**
 * register a accelerator driver for use.
 * @param driver pointer to the driver
 * @return `CONTROL_OK` if it went well.
 * `CONTROL_FAIL` if not.
 */
control_err_t control_register_driver(ControlAcceleratorDriver *driver);

/**
 * unregister a accelerator driver for use.
 * @param driver pointer to the driver
 * @return `CONTROL_OK` if it went well.
 * `CONTROL_FAIL` if not.
 */
control_err_t control_unregister_driver(ControlAcceleratorDriver *driver);
// TODO implement

/**
 * Calibarte the sensors, aka set 0 point. make sure that the device is standing still and is level for 400ms.
 */
void control_calibrate_sensors();

/**
 * test whenever the sensors are connected, sensor should be horizontal and facing up.
 * @return 1 if all is well; 0 if not.
 */
uint8_t control_test_sensor();


/**
 * Set the speed of X is cm per second.
 * @param mm_per_second set new speed.
 * @return control_err_t
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 */
// TODO documentatie
void control_set_speed(control_axis_mask_t axisMask, uint16_t speed, control_direction_t direction = CONTROL_DIRECTION_PLUS);
void control_set_velocity(control_axis_mask_t axisMask, uint16_t mm_per_second, control_direction_t direction = CONTROL_DIRECTION_PLUS);
void control_set_degree(control_axis_mask_t axisMask, uint16_t degree, control_direction_t direction = CONTROL_DIRECTION_PLUS);

uint16_t control_get_speed(control_axis_mask_t single_axis);
uint16_t control_get_velocity(control_axis_mask_t single_axis);
uint16_t control_get_degree(control_axis_mask_t single_axis);
control_direction_t control_get_direction(control_axis_mask_t single_axis);

#endif //ARUNA_CONTROL_H
