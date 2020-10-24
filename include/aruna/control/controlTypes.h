//
// Created by noeel on 6-7-19.
//

#ifndef ARUNA_CONTROLTYPES_H
#define ARUNA_CONTROLTYPES_H

#include <stdint.h>
#include "stddef.h"

namespace aruna { namespace control {

        enum class axis_mask_t: uint8_t {
            X = (1 << 0),
            Y = (1 << 1),
            Z = (1 << 2),
            ROLL = (1 << 3),
            YAW = (1 << 4),
            PITCH = (1 << 5),

            ALL = 63,
            MAX = 6,
            NONE = 0,
        };

        enum class damping_t {
            DISABLE = 0,
            KEEP_VELOCITY = 0x01,
            KEEP_DEGREE = 0x02,
        };

        // status of control
        enum class status_t {
            RUNNING = 0,
            STOPPED = 1,
        };

//        TODO direction mask?
        enum class direction_t: uint8_t {
            PLUS = 0,
            MIN = 1,
            BOTH = 2,
            NONE = 4,
            MAX = 4
        };


        template<typename T>
        struct axis_t {
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
                    case (uint8_t) axis_mask_t::X:
                        return x;
                        break;
                    case (uint8_t) axis_mask_t::Y:
                        return y;
                        break;
                    case (uint8_t) axis_mask_t::Z:
                        return z;
                        break;
                    case (uint8_t) axis_mask_t::ROLL:
                        return roll;
                        break;
                    case (uint8_t) axis_mask_t::YAW:
                        return yaw;
                        break;
                    case (uint8_t) axis_mask_t::PITCH:
                        return pitch;
                        break;
                }
            }
        };
    }
}
#endif //ARUNA_CONTROLTYPES_H
