//
// Created by noeel on 18-01-21.
//

#include "gmock/gmock.h"
#include "aruna/arunaTypes.h"
#include "aruna/movement/movementTypes.h"
#include "aruna/movement/Actuator.h"
#include "MockActuator.h"
#include "catch2/catch.hpp"

using namespace aruna;
using namespace aruna::movement;
using ::testing::Return;
using ::testing::_;

SCENARIO("Actuator", "[movement]") {
    GIVEN("actuator mock object") {
        MockActuator actuator;
        err_t ret = err_t::ALREADY_STARTED;
        int16_t speed = 1000;
        THEN("startup values should be 0") {
            REQUIRE(actuator.startup_error == err_t::NOT_STARTED);
            REQUIRE((axis_mask_t) 0 == actuator.get_axis());
            REQUIRE( 0 == actuator.get_speed());
        }
        WHEN("setting x axis"){
            actuator.set_axis(axis_mask_t::X);
            AND_WHEN("setting speed for y axis") {
                err_t ret = actuator.set(axis_mask_t::Y, speed);
                THEN("return should be OK") {
                    REQUIRE(ret == err_t::OK);
                }
                AND_THEN("speed should remain 0") {
                    REQUIRE(0 == actuator.get_speed());
                }
            }
        }
        WHEN("setting Y as active axis") {
            actuator.set_axis(axis_mask_t::Y);
            AND_WHEN("setting y axis") {
                err_t ret = actuator.set(axis_mask_t::Y, speed);
                THEN("return should be OK") {
                    REQUIRE(ret == err_t::OK);
                }
                AND_THEN("speed should be set") {
                    REQUIRE(speed == actuator.get_speed());
                }
            }
        }
        WHEN("setting mask") {
            //    TODO this endless casting is making me dissy!
            axis_mask_t mask = (axis_mask_t)((int)axis_mask_t::X | (int)axis_mask_t::ROLL);
            err_t ret = actuator.set_axis(mask);
            THEN("mask should return OK") {
                REQUIRE(ret == err_t::OK);
            }
            AND_THEN("mask should be stored") {
                REQUIRE(mask == actuator.get_axis());
            }
        }
    }
}