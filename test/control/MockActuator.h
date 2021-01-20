//
// Created by noeel on 18-01-21.
//
#include "gmock/gmock.h"
#include "aruna/arunaTypes.h"
#include "aruna/control/controlTypes.h"
#include "aruna/control/Actuator.h"

#ifndef ARUNA_MOCKACTUATOR_H
#define ARUNA_MOCKACTUATOR_H

class MockActuator: public aruna::control::Actuator {
public:
    MOCK_METHOD(aruna::err_t,  _set, (aruna::control::axis_mask_t axisMask, int16_t speed), (override));
};


#endif //ARUNA_MOCKACTUATOR_H
