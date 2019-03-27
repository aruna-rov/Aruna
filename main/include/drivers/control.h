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
    CONTROL_PITCH = (1 << 5)
} control_mode_mask_t;

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
    DAMP_ALLOW,
    DAMP_STAY_AT_POSITION,
    DAMP_STAND_STILL
} control_damping_t;

// status of control
typedef enum  {
    CONTROL_RUNNING,
    CONTROL_STOPPED,
} control_status_t;


typedef uint8_t control_mode_t;

// variables


// functions


/**
 * Set the speed of X is cm per second.
 * @param cm_per_second set new speed.
 * @return control_err_t
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 */
control_err_t control_set_X_velocity(float cm_per_second);

/**
 * Set the speed of the engine/moter etc. directly
 * @param speed, how fast the motor must spin (min-max must be defined by the motor hardware interface)
 * @return control_err_t
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` speed overflow.
 *  * `CONTROL_ERR_UNDERFLOW` speed underflow.
 */
static control_err_t set_X_speed(int speed);

/**
 * get velocity of X
 * @return current cm/s
 */
float control_get_X_velocity();

/**
 * Set the speed of Y is cm per second.
 * @param cm_per_second set new speed.
 * @return control_err_t
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 */
control_err_t control_set_Y_velocity(float cm_per_second);

/**
 * Set the speed of the engine/moter etc. directly
 * @param speed, how fast the motor must spin (min-max must be defined by the motor hardware interface)
 * @return control_err_t
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` speed overflow.
 *  * `CONTROL_ERR_UNDERFLOW` speed underflow.
 */
static control_err_t set_Y_speed(int speed);

/**
 * get velocity of Y
 * @return current cm/s
 */
float control_get_Y_velocity();

/**
 * Set the speed of Z is cm per second.
 * @param cm_per_second set new speed.
 * @return control_err_t
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 */
control_err_t control_set_Z_velocity(float cm_per_second);

/**
 * Set the speed of the engine/moter etc. directly
 * @param speed, how fast the motor must spin (min-max must be defined by the motor hardware interface)
 * @return control_err_t
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` speed overflow.
 *  * `CONTROL_ERR_UNDERFLOW` speed underflow.
 */
static control_err_t set_Z_speed(int speed);

/**
 * get velocity of Z
 * @return current cm/s
 */
float control_get_Z_velocity();

/**
 * Set the roll degree
 * @param deg (0-360)
 * @return control_err_t
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` if deg > 360.
 */
control_err_t control_set_roll_deg(unsigned short deg);

/**
 * Set the speed of the engine/moter etc. directly
 * @param speed, how fast the motor must spin (min-max must be defined by the motor hardware interface)
 * @return control_err_t
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` speed overflow.
 *  * `CONTROL_ERR_UNDERFLOW` speed underflow.
 */
static control_err_t set_roll_speed(int speed);

/**
 * get current deg of roll (could be different than desired)
 * @return 0-360
 */
unsigned short control_get_roll_deg();

/**
 * Set the yaw degree
 * @param deg (0-360)
 * @return control_err_t
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` if deg > 360.
 */
control_err_t control_set_yaw_deg(unsigned short deg);

/**
 * Set the speed of the engine/moter etc. directly
 * @param speed, how fast the motor must spin (min-max must be defined by the motor hardware interface)
 * @return control_err_t
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` speed overflow.
 *  * `CONTROL_ERR_UNDERFLOW` speed underflow.
 */
static control_err_t set_yaw_speed(int speed);

/**
 * get current deg of yaw (could be different that desired)
 * @return 0-360
 */
unsigned short control_get_yaw_deg();

/**
 * Set the pitch degree
 * @param deg (0-360)
 * @return control_err_t
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` if deg > 360.
 */
control_err_t control_set_pitch_deg(unsigned short deg);

/**
 * Set the speed of the engine/moter etc. directly
 * @param speed, how fast the motor must spin (min-max must be defined by the motor hardware interface)
 * @return control_err_t
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` speed overflow.
 *  * `CONTROL_ERR_UNDERFLOW` speed underflow.
 */
static control_err_t set_pitch_speed(int speed);

/**
 * get current deg of pitch (could be different that desired)
 * @return 0-360
 */
unsigned short control_get_pitch_deg();

/**
 * Get active modus (X,Y,Z,yawn,pitch,roll)
 * @return `control_mode_t` with a bit high on enabled modus. Use `control_mode_mask_t` to decipher.
 */
control_mode_t control_get_active_modes();

/**
 * initialise control and communicate with hardware for active modes.
 * @return control_err_t
 *  * `CONTROL_OK` started successfully,
 *  * `CONTROL_ERR_NO_HARDWARE_FOUND` could not find andy drivers,
 *  * `CONTROL_ERR_NO_ACTIVE_MODULES` no module reported active,
 *  * `CONTROL_ERR_ALREADY_STARTED` control is already active,
 */
control_err_t control_start();

/**
 * Stop all motors and free all processes.
 * @return control_err_t
 *  * `CONTROL_OK` stopped successfully.
 *  * `CONTROL_ERR_NOT_STARTED` control was not started yet.
 */
control_err_t control_stop();

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
void control_set_damping(control_damping_t damp);

/**
 * register a accelerator driver for use.
 * @param driver pointer to the driver
 * @return `CONTROL_OK` if it went well.
 * `CONTROL_FAIL` if not.
 */
control_err_t control_register_driver(ControlAcceleratorDriver *driver);

#endif //ARUNA_CONTROL_H
