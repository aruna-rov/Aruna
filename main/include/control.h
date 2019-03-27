//
// Created by noeel on 6-1-19.
//

#ifndef ARUNA_CONTROL_H
#define ARUNA_CONTROL_H

typedef enum {
    X = 1,
    Y = 2,
    Z = 4,
    ROLL = 8,
    YAW = 16,
    PITCH = 32
} control_mode_mask;

typedef enum {
    CONTROL_OK,
    CONTROL_ERR_NO_HARDWARE_FOUND,
    CONTROL_ERR_NO_ACTIVE_MODULES,
    CONTROL_ERR_NOT_STARTED,
    CONTROL_ERR_ALREADY_STARTED,
    CONTROL_ERR_MODE_NOT_ACTIVE,
    CONTROL_ERR_HARDWARE_FAILURE,
    CONTROL_ERR_OVERFLOW,
    CONTROL_ERR_UNDERFLOW
} control_err;

typedef char control_mode_t;

/**
 * Set the speed of X is cm per second.
 * @param cm_per_second set new speed.
 * @return control_err
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 */
control_err control_set_X_velocity(float cm_per_second);

/**
 * Set the speed of the engine/moter etc. directly
 * @param speed, how fast the motor must spin (min-max must be defined by the motor hardware interface)
 * @return control_err
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` speed overflow.
 *  * `CONTROL_ERR_UNDERFLOW` speed underflow.
 */
static control_err set_X_speed(int speed);

/**
 * get velocity of X
 * @return current cm/s
 */
float control_get_X_velocity();

/**
 * Set the speed of Y is cm per second.
 * @param cm_per_second set new speed.
 * @return control_err
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 */
control_err control_set_Y_velocity(float cm_per_second);

/**
 * Set the speed of the engine/moter etc. directly
 * @param speed, how fast the motor must spin (min-max must be defined by the motor hardware interface)
 * @return control_err
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` speed overflow.
 *  * `CONTROL_ERR_UNDERFLOW` speed underflow.
 */
static control_err set_Y_speed(int speed);

/**
 * get velocity of Y
 * @return current cm/s
 */
float control_get_Y_velocity();

/**
 * Set the speed of Z is cm per second.
 * @param cm_per_second set new speed.
 * @return control_err
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 */
control_err control_set_Z_velocity(float cm_per_second);

/**
 * Set the speed of the engine/moter etc. directly
 * @param speed, how fast the motor must spin (min-max must be defined by the motor hardware interface)
 * @return control_err
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` speed overflow.
 *  * `CONTROL_ERR_UNDERFLOW` speed underflow.
 */
static control_err set_Z_speed(int speed);

/**
 * get velocity of Z
 * @return current cm/s
 */
float control_get_Z_velocity();

/**
 * Set the roll degree
 * @param deg (0-360)
 * @return control_err
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` if deg > 360.
 */
control_err control_set_roll_deg(unsigned short deg);

/**
 * Set the speed of the engine/moter etc. directly
 * @param speed, how fast the motor must spin (min-max must be defined by the motor hardware interface)
 * @return control_err
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` speed overflow.
 *  * `CONTROL_ERR_UNDERFLOW` speed underflow.
 */
static control_err set_roll_speed(int speed);

/**
 * get current deg of roll (could be different that desired)
 * @return 0-360
 */
unsigned short control_get_roll_deg();

/**
 * Set the yaw degree
 * @param deg (0-360)
 * @return control_err
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` if deg > 360.
 */
control_err control_set_yaw_deg(unsigned short deg);

/**
 * Set the speed of the engine/moter etc. directly
 * @param speed, how fast the motor must spin (min-max must be defined by the motor hardware interface)
 * @return control_err
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` speed overflow.
 *  * `CONTROL_ERR_UNDERFLOW` speed underflow.
 */
static control_err set_yaw_speed(int speed);

/**
 * get current deg of yaw (could be different that desired)
 * @return 0-360
 */
unsigned short control_get_yaw_deg();

/**
 * Set the pitch degree
 * @param deg (0-360)
 * @return control_err
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` if deg > 360.
 */
control_err control_set_pitch_deg(unsigned short deg);

/**
 * Set the speed of the engine/moter etc. directly
 * @param speed, how fast the motor must spin (min-max must be defined by the motor hardware interface)
 * @return control_err
 *  * `CONTROL_OK` if is was a success.
 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
 *  * `CONTROL_ERR_OVERFLOW` speed overflow.
 *  * `CONTROL_ERR_UNDERFLOW` speed underflow.
 */
static control_err set_pitch_speed(int speed);

/**
 * get current deg of pitch (could be different that desired)
 * @return 0-360
 */
unsigned short control_get_pitch_deg();

/**
 * Get active modus (X,Y,Z,yawn,pitch,roll)
 * @return `control_mode_t` with a big high on enabled modus. Use `control_mode_mask` to decipher.
 */
control_mode_t control_get_active_modes();

/**
 * initialise control and communicate with hardware for active modes.
 * @return control_err
 *  * `CONTROL_OK` started successfully,
 *  * `CONTROL_ERR_NO_HARDWARE_FOUND` could not find andy drivers,
 *  * `CONTROL_ERR_NO_ACTIVE_MODULES` no module reported active,
 *  * `CONTROL_ERR_ALREADY_STARTED` control is already active,
 */
control_err control_start();

/**
 * Stop all motors and free all processes.
 * @return control_err
 *  * `CONTROL_OK` stopped successfully.
 *  * `CONTROL_ERR_NOT_STARTED` control was not started yet.
 */
control_err control_stop();

#endif //ARUNA_CONTROL_H
