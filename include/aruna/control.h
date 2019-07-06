//
// Created by noeel on 6-1-19.
//

#ifndef ARUNA_CONTROL_H
#define ARUNA_CONTROL_H

#include <stdint.h>
#include "controlTypes.h"
#include "aruna/drivers/control/ControlActuatorDriver.h"
namespace aruna { namespace control {
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
 *  * `CONTROL_DAMP_DISABLE` will disable all damping for that axis
 *  * `CONTROL_DAMP_KEEP_VELOCITY` ROV will try to keep the current velocity
 *  * `CONTROL_DAMP_KEEP_DEGREE` ROV will try to keep the current degree (only row, yaw and pitch)
 * @param damp control_damping_t, mode to set damping to.
 * @param axisMask axis to apply damping to
 */
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
control_err_t control_register_driver(drivers::control::ControlActuatorDriver *driver);

/**
 * unregister a accelerator driver for use.
 * @param driver pointer to the driver
 * @return `CONTROL_OK` if it went well.
 * `CONTROL_FAIL` if not.
 */
control_err_t control_unregister_driver(drivers::control::ControlActuatorDriver*driver);
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
 * Set the speed of the motors directly
 * @param axisMask, multiple axis to apply speed to.
 * @param speed, speed of the motors
 * @param direction, direction to go to.
 */
void control_set_speed(control_axis_mask_t axisMask, uint16_t speed, control_direction_t direction = CONTROL_DIRECTION_PLUS);

/**
 * Set the target velocity of the ROV
 * @param axisMask axis to apply the velocity to
 * @param mm_per_second how fast
 * @param direction direction to go to (default is plus)
 */
void control_set_velocity(control_axis_mask_t axisMask, uint16_t mm_per_second, control_direction_t direction = CONTROL_DIRECTION_PLUS);

/**
 * set the degree of an axis (only applies to yaw, pitch and roll)
 * @param axisMask multiple axis to apply this to
 * @param degree degree (65535 is maximum)
 * @param direction not used, exist for pointer compatability
 */
void control_set_degree(control_axis_mask_t axisMask, uint16_t degree, control_direction_t direction = CONTROL_DIRECTION_PLUS);

/**
 * Get the speed of an axis.
 * @param single_axis one axis at a time
 * @return speed.
 */
uint16_t control_get_speed(control_axis_mask_t single_axis);

/**
 * get the current velocity
 * @param single_axis of target axis
 * @return mm_second how fast we are going
 */
uint16_t control_get_velocity(control_axis_mask_t single_axis);

/**
 * get the angle of an axis (pitch, roll and yaw only)
 * @param single_axis of target axis
 * @return degree of axis (65535 = 360 deg)
 */
uint16_t control_get_degree(control_axis_mask_t single_axis);

/**
 * Get the direction that an axis is spinning at.
 * @param single_axis target axis
 * @return direction
 */
control_direction_t control_get_direction(control_axis_mask_t single_axis);
}}
#endif //ARUNA_CONTROL_H
