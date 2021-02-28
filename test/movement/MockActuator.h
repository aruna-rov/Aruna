//
// Created by noeel on 18-01-21.
//
#include "gmock/gmock.h"
#include "aruna/arunaTypes.h"
#include "aruna/movement/movementTypes.h"
#include "aruna/movement/Actuator.h"

#ifndef ARUNA_MOCKACTUATOR_H
#define ARUNA_MOCKACTUATOR_H

class MockActuator: public aruna::movement::Actuator {
public:
    MOCK_METHOD(aruna::err_t, _set, (aruna::movement::axis_mask_t axisMask, int16_t speed), (override));
};


#endif //ARUNA_MOCKACTUATOR_H
