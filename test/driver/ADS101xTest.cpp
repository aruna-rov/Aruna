//
// Created by noeel on 25-04-21.
//

#include <gmock/gmock.h>
#include <catch2/catch.hpp>
#include <iostream>
#include "aruna/driver/ADS101x.h"
#include "MockI2C_master.h"

using namespace aruna;
using namespace aruna::driver;
using ::testing::_;

#ifdef ARUNA_ENABLE_FTDI

#include "aruna/driver/port/cross_platform/FT_I2C.h"

SCENARIO("Reading values from reallife ADS101x using FTDI chip") {
    GIVEN("ADS101x object with FT I²C master") {
        mpsse_context *myMpsseObject = MPSSE(I2C, ONE_HUNDRED_KHZ, MSB);
        FT_I2C FT(myMpsseObject);
        ADS101x adc(ADS101x::MUX::AINp0_AINnGND, &FT);
        THEN("connection should succeed") {
//            adc.isConnected() // TODO
        }WHEN("reading voltage") {
            err_t err;
            int32_t mv = 0;
            err = adc.read_voltage(mv);
            THEN("no error should accour") {
                REQUIRE(err_to_char.at(err) == err_to_char.at(err_t::OK));
            }AND_THEN("microvolt should be store in variable") {
                std::cout << "measured: " << mv << "mV" << std::endl;
                // TODO can't measure if correct.
            }
        }
    }
}

#endif

SCENARIO("ADS101x value convertion") {
    GIVEN("ADS101x object with spoofed I2C") {
        MockI2C_master spoof;
        ADS101x adc(ADS101x::MUX::AINp0_AINnGND, &spoof);
        WHEN("Reading raw dummy input") {
            uint8_t data[3][2] = {
                    {0b00000111, 0b11111111}, // 2047
                    {0b00001000, 0b00000001}, // 2047
//                    {0b00001000, 0b00000000}, // -2048
//                    {0x2F, 0x90},
                    {0x00, 0x00}, // 0
//                    {0xFF, 0xFF},
            };
            int32_t expect[3] = {
                    INT32_MAX,
                    INT32_MIN,
                    0,
            };
            EXPECT_CALL(spoof, _read(_, _, _, _))
                    .WillOnce([data](uint8_t address, uint8_t reg, uint8_t* buffer, size_t buffer_size) {
                        memcpy(buffer, data[0], buffer_size);
                        return err_t::OK;
                    })
                    .WillOnce([data](uint8_t address, uint8_t reg, uint8_t* buffer, size_t buffer_size) {
                        memcpy(buffer, data[1], buffer_size);
                        return err_t::OK;
                    })
                    .WillOnce([data](uint8_t address, uint8_t reg, uint8_t* buffer, size_t buffer_size) {
                        memcpy(buffer, data[2], buffer_size);
                        return err_t::OK;
                    });
            // make sure the values are zero.
            int32_t *mv = static_cast<int32_t *>(calloc(sizeof(int32_t), 3));
            adc.read_raw(mv[0]);
            adc.read_raw(mv[1]);
            adc.read_raw(mv[2]);
            THEN("convertion from two compliments 12bit to int32 should be correct") {
                for (int i = 0; i < 3; ++i) {
                    int32_t conversion = (int16_t) (data[i][0] << 8 | (data[i][1] & 0xFF));
                    conversion = conversion >> 4;
                    int32_t raw = ((conversion * (INT32_MAX - INT32_MIN)) / 2047) + INT32_MIN;
                    REQUIRE(expect[i] == mv[i]);
                }
            }
        }
    }
}