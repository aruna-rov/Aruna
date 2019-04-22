//
// Created by noeel on 30-3-19.
//

#ifndef ARUNA_L293D_H
#define ARUNA_L293D_H

#include "drivers/control/ControlActuatorDriver.h"

class L293D: public ControlActuatorDriver  {
public:
    control_err_t start() override;
    control_err_t stop() override;
	control_axis_mask_t get_axis() override;

	control_err_t set(control_axis_mask_t axisMask, uint16_t speed, control_direction_t direction) override;
private:
};


#endif //ARUNA_L293D_H
