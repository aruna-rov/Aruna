//
// Created by noeel on 18-01-21.
//

#include "gtest/gtest.h"
#include "MockActuator.h"
#include "aruna/control/ActuatorSet.h"

using namespace aruna;
using namespace aruna::control;
using ::testing::Return;
using ::testing::_;

class ActuatorSetTest : public ::testing::Test {
protected:
    MockActuator drv1;
    MockActuator drv2;
    MockActuator drv3;
    MockActuator drv4;
    err_t drv1_startup_error = err_t::BUFFER_OVERFLOW;

    int16_t speed[7] = {-4500, 500, 3000, -8003, 0, INT16_MAX, INT16_MIN};
    size_t speed_size = 7;

    axis_mask_t all_axis_combined = axis_mask_t(
            (int) axis_mask_t::X | (int) axis_mask_t::Y | (int) axis_mask_t::YAW | (int) axis_mask_t::Z |
            (int) axis_mask_t::PITCH | (int) axis_mask_t::ROLL);
    ActuatorSet *actuatorSet;

    ActuatorSet::transform_t trans[4] = {
            {
                    .driver = &drv1,
                    .transform_to = axis_mask_t::Z,
                    .flip_direction = true,
                    .axis = axis_mask_t::X,
                    .speed_percentage = 50.0,
            },
            {
                    .driver = &drv2,
                    .transform_to = axis_mask_t::Z,
                    .flip_direction = false,
                    .axis = axis_mask_t::YAW,
                    .speed_percentage = 100.f,
            },
            {
                    .driver = &drv3,
                    .transform_to = axis_mask_t::ROLL,
                    .flip_direction = false,
                    .axis = axis_mask_t::Z,
                    .speed_percentage = 200.f,
            },
            {
                    .driver = &drv4,
                    .transform_to = axis_mask_t::ROLL,
                    .flip_direction = false,
                    .axis = axis_mask_t::PITCH,
                    .speed_percentage = -100.f,
            }
    };

    void SetUp() override {

        drv1.set_axis(axis_mask_t::X);
        drv2.set_axis((axis_mask_t) ((int) axis_mask_t::Y | (int) axis_mask_t::YAW));
        drv3.set_axis(axis_mask_t::Z);
        drv4.set_axis(axis_mask_t::PITCH);

        drv1.startup_error = drv1_startup_error;
        drv2.startup_error = err_t::OK;
        drv3.startup_error = err_t::OK;
        drv4.startup_error = err_t::OK;

        actuatorSet = new ActuatorSet(trans, 4);
    }
//    TODO write TearDown() that will remove ActuatorSet from log.
};


TEST_F(ActuatorSetTest, startup) {
    EXPECT_EQ(drv1_startup_error, actuatorSet->startup_error);
}

TEST_F(ActuatorSetTest, axis) {
    EXPECT_EQ(all_axis_combined, actuatorSet->get_axis());
}

TEST_F(ActuatorSetTest, set_error) {

//    Test transform_to
//    Test correct driver called
//    Test flip_direction
//    Test speed_percentage
//    Test return correct error
//    Test correct driver called
    EXPECT_CALL(drv1, _set(axis_mask_t::X, -1 * (speed[1] / 2.f)))
            .Times(1)
            .WillOnce(Return(err_t::DRIVER_EXISTS));
    EXPECT_CALL(drv2, _set(axis_mask_t::YAW, speed[1]))
            .Times(1)
            .WillOnce(Return(err_t::OK));
    EXPECT_CALL(drv3, _set(axis_mask_t::Z, speed[1]))
            .Times(1)
            .WillOnce(Return(err_t::OK));
    EXPECT_CALL(drv4, _set(_, _))
            .Times(0);

    EXPECT_EQ(err_t::DRIVER_EXISTS, actuatorSet->set(axis_mask_t::Z, speed[1]));

}

TEST_F(ActuatorSetTest, set_speed) {
    //    Test speeds
    for (int i = 0; i < speed_size; ++i) {
        EXPECT_CALL(drv1, _set(axis_mask_t::X, -1 * (speed[i] / 2.f)))
                .Times(1)
                .WillOnce(Return(err_t::OK))
                .RetiresOnSaturation();
        EXPECT_CALL(drv2, _set(axis_mask_t::YAW, speed[i]))
                .Times(1)
                .WillOnce(Return(err_t::OK))
                .RetiresOnSaturation();
        EXPECT_CALL(drv3, _set(axis_mask_t::Z, speed[i]))
                .Times(1)
                .WillOnce(Return(err_t::OK))
                .RetiresOnSaturation();
        EXPECT_CALL(drv4, _set(_, _))
                .Times(0);

        EXPECT_EQ(err_t::OK, actuatorSet->set(axis_mask_t::Z, speed[i]));
    }
}

TEST_F(ActuatorSetTest, set_percentage) {

    int32_t speed_drv3;
//    Test if percentage is over- and under-flow protected.
    for (int i = 0; i < speed_size; ++i) {
        speed_drv3 = speed[i] * 2;
        speed_drv3 = fmax(speed_drv3, INT16_MIN);
        speed_drv3 = fmin(speed_drv3, INT16_MAX);
        EXPECT_CALL(drv1, _set(_, _))
                .Times(0);
        EXPECT_CALL(drv2, _set(_, _))
                .Times(0);
        EXPECT_CALL(drv3, _set(axis_mask_t::Z, speed_drv3))
                .Times(1)
                .WillOnce(Return(err_t::OK))
                .RetiresOnSaturation();
        EXPECT_CALL(drv4, _set(axis_mask_t::PITCH, fmin(INT16_MAX, -1 * speed[i])))
                .Times(1)
                .WillOnce(Return(err_t::OK))
                .RetiresOnSaturation();
        EXPECT_EQ(err_t::OK, actuatorSet->set(axis_mask_t::ROLL, speed[i]));
    }

}

TEST_F(ActuatorSetTest, set_native) {
//    Test to see if native `sets` still work
    EXPECT_CALL(drv1, _set(axis_mask_t::X, speed[1]))
            .Times(1)
            .WillOnce(Return(err_t::OK));
    EXPECT_CALL(drv2, _set(axis_mask_t::Y, speed[1]))
            .Times(1)
            .WillOnce(Return(err_t::OK));
    EXPECT_CALL(drv2, _set((axis_mask_t) ((int)axis_mask_t::Y | (int) axis_mask_t::YAW), speed[1]))
            .Times(1)
            .WillOnce(Return(err_t::OK));
    EXPECT_CALL(drv2, _set(axis_mask_t::YAW, speed[1]))
            .Times(1)
            .WillOnce(Return(err_t::OK));
    EXPECT_CALL(drv3, _set(axis_mask_t::Z, speed[1]))
            .Times(0);
    EXPECT_CALL(drv4, _set(axis_mask_t::PITCH, speed[1]))
            .Times(1)
            .WillOnce(Return(err_t::OK));

    EXPECT_EQ(err_t::OK, actuatorSet->set(axis_mask_t::X, speed[1]));
    EXPECT_EQ(err_t::OK, actuatorSet->set(axis_mask_t::Y, speed[1]));
    EXPECT_EQ(err_t::OK, actuatorSet->set((axis_mask_t) ((int)axis_mask_t::Y | (int) axis_mask_t::YAW), speed[1]));
    EXPECT_EQ(err_t::OK, actuatorSet->set(axis_mask_t::YAW, speed[1]));
    EXPECT_EQ(err_t::OK, actuatorSet->set(axis_mask_t::PITCH, speed[1]));

}