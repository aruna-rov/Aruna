//
// Created by noeel on 30-3-19.
//

#ifndef ARUNA_L293D_H
#define ARUNA_L293D_H

#include "aruna/controlTypes.h"
#include "ControlActuatorDriver.h"

namespace aruna { namespace drivers { namespace control {

class L293D: public ControlActuatorDriver  {
public:
    aruna::control::control_err_t start() override;
    aruna::control::control_err_t stop() override;
    aruna::control::control_axis_mask_t get_axis() override;

    aruna::control::control_err_t set(aruna::control::control_axis_mask_t axisMask, uint16_t speed, aruna::control::control_direction_t direction) override;
private:
};
}}}

#endif //ARUNA_L293D_H
