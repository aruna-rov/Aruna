//
// Created by noeel on 30-3-19.
//

#ifndef ARUNA_L293D_H
#define ARUNA_L293D_H

#include "drivers/control/ControlAcceleratorDriver.h"

class L293D: public ControlAcceleratorDriver  {
public:
    control_err_t start() override;
    control_err_t stop() override;
    control_mode_t get_control_mode() override;

    control_err_t set_X_speed(uint32_t speed, control_direction_t direction) override;
private:
};


#endif //ARUNA_L293D_H
