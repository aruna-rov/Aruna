//
// Created by noeel on 27-3-19.
//

#ifndef ARUNA_CONTROLACCELERATORDRIVER_H
#define ARUNA_CONTROLACCELERATORDRIVER_H

#include "control.h"

class ControlAcceleratorDriver {
public:
    /**
     * start the driver
     */
    virtual void start();

    /**
     * stop the driver
     */
    virtual void stop();

    /**
     * get the control modes that this driver supports.
     * @return control_mode mask ORed.
     */
    virtual control_mode_t get_control_mode() = 0;

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
    virtual control_err_t set_X_speed(int speed) {
        return CONTROL_ERR_MODE_NOT_ACTIVE;
    };

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
    virtual control_err_t set_Y_speed(int speed) {
        return CONTROL_ERR_MODE_NOT_ACTIVE;
    };

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
    virtual control_err_t set_Z_speed(int speed) {
        return CONTROL_ERR_MODE_NOT_ACTIVE;
    };

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
    virtual control_err_t set_roll_speed(int speed) {
        return CONTROL_ERR_MODE_NOT_ACTIVE;
    };

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
    virtual control_err_t set_yaw_speed(int speed) {
        return CONTROL_ERR_MODE_NOT_ACTIVE;
    };

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
    virtual control_err_t set_pitch_speed(int speed) {
        return CONTROL_ERR_MODE_NOT_ACTIVE;
    };

};
#endif //ARUNA_CONTROLACCELERATORDRIVER_H
