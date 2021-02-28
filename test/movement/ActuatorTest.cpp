//
// Created by noeel on 18-01-21.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "aruna/arunaTypes.h"
#include "aruna/movement/movementTypes.h"
#include "aruna/movement/Actuator.h"
#include "MockActuator.h"

using namespace aruna;
using namespace aruna::movement;
using ::testing::Return;
using ::testing::_;


TEST(Actuator, speed) {
    MockActuator actuator;
    err_t ret = err_t::ALREADY_STARTED;
    int16_t speed = 1000;
    EXPECT_CALL(actuator, _set( _, speed))
            .Times(2)
            .WillOnce(Return(ret))
            .WillOnce(Return(err_t::OK));

    actuator.set_axis(axis_mask_t::X);
    EXPECT_EQ(err_t::OK, actuator.set(axis_mask_t::Y, speed));
    EXPECT_EQ(0, actuator.get_speed());
    actuator.set_axis(axis_mask_t::Y);
    EXPECT_EQ(ret, actuator.set(axis_mask_t::Y, speed));
    EXPECT_EQ(0, actuator.get_speed());
    EXPECT_EQ(err_t::OK, actuator.set(axis_mask_t::Y, speed));
    EXPECT_EQ(speed, actuator.get_speed());

}

TEST(Actuator, axis) {
    MockActuator actuator;
    //    TODO this endless casting is making me dissy!
    axis_mask_t mask = (axis_mask_t)((int)axis_mask_t::X | (int)axis_mask_t::ROLL);
    ASSERT_EQ(err_t::OK,actuator.set_axis(mask));

    EXPECT_EQ(mask, actuator.get_axis());

}

TEST(Actuator, start_values) {
    MockActuator actuator;
    EXPECT_EQ(actuator.startup_error, err_t::NOT_STARTED);
    EXPECT_EQ((axis_mask_t) 0,actuator.get_axis());
    EXPECT_EQ( 0,actuator.get_speed());

}