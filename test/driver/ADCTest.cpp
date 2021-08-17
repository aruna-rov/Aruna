//
// Created by noeel on 25-04-21.
//

#include "gmock/gmock.h"
#include <catch2/catch.hpp>
#include "MockADC.h"

using ::testing::_;
using ::testing::Return;
using namespace aruna;


SCENARIO("converting raw to voltage") {
    GIVEN("ADC object 3300mv reference") {
        MockADC adc(3300, 12);
        WHEN("converting raw to voltage") {
            int32_t data[3] = {
                    INT32_MIN,
                    INT32_MAX,
                    0,
            };
            int32_t expect[3] = {
                    -3300,
                    3300,
                    0
            };
            EXPECT_CALL(adc, _read(_))
                    .WillOnce([data](int32_t &raw) {
                        raw = data[0];
                        return err_t::OK;
                    }).WillOnce([data](int32_t &raw) {
                        raw = data[1];
                        return err_t::OK;
                    }).WillOnce([data](int32_t &raw) {
                        raw = data[2];
                        return err_t::OK;
                    })
                    ;
            // make sure the values are zero.
            int32_t *mv = static_cast<int32_t *>(calloc(sizeof(int32_t), 3));
            adc.read_voltage(mv[0]);
            adc.read_voltage(mv[1]);
            adc.read_voltage(mv[2]);
            THEN("convertion raw to voltage should be correct") {
                for (int i = 0; i < 3; ++i) {
                    REQUIRE(expect[i] == mv[i]);
                }
            }
        }
    }
}