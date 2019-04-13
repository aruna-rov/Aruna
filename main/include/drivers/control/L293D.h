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
	control_axis_mask_t get_axis() override;

	void set(control_axis_mask_t modes, uint16_t speed, control_direction_t direction) override;
private:
};


#endif //ARUNA_L293D_H
