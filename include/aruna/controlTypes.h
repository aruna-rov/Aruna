//
// Created by noeel on 6-7-19.
//

#ifndef ARUNA_CONTROLTYPES_H
#define ARUNA_CONTROLTYPES_H

namespace aruna { namespace control {

        typedef enum {
            CONTROL_AXIS_MASK_X = (1 << 0),
            CONTROL_AXIS_MASK_Y = (1 << 1),
            CONTROL_AXIS_MASK_Z = (1 << 2),
            CONTROL_AXIS_MASK_ROLL = (1 << 3),
            CONTROL_AXIS_MASK_YAW = (1 << 4),
            CONTROL_AXIS_MASK_PITCH = (1 << 5),

            CONTROL_AXIS_MASK_ALL = 63,
            CONTROL_AXIS_MASK_MAX = 6,

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
            CONTROL_DAMP_DISABLE = 0,
            CONTROL_DAMP_KEEP_VELOCITY = 0x01,
            CONTROL_DAMP_KEEP_DEGREE = 0x02,
        } control_damping_t;

// status of control
        typedef enum {
            CONTROL_RUNNING = 0,
            CONTROL_STOPPED = 1,
        } control_status_t;

        typedef enum {
            CONTROL_DIRECTION_PLUS = 0,
            CONTROL_DIRECTION_MIN = 1
        } control_direction_t;


        template<typename T>
        struct control_axis_t {
            T x;
            T y;
            T z;
            T roll;
            T yaw;
            T pitch;

            T &operator[](size_t i) {
//		TODO moet dit geen pointer returnen?
                switch (i) {
                    default:
                    case CONTROL_AXIS_MASK_X:
                        return x;
                        break;
                    case CONTROL_AXIS_MASK_Y:
                        return y;
                        break;
                    case CONTROL_AXIS_MASK_Z:
                        return z;
                        break;
                    case CONTROL_AXIS_MASK_ROLL:
                        return roll;
                        break;
                    case CONTROL_AXIS_MASK_YAW:
                        return yaw;
                        break;
                    case CONTROL_AXIS_MASK_PITCH:
                        return pitch;
                        break;
                }
            }
        };
    }
}
#endif //ARUNA_CONTROLTYPES_H
