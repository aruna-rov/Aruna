//
// Created by noeel on 23-04-21.
//
#ifdef ARUNA_ENABLE_FTDI
#include <catch2/catch.hpp>
#include <iostream>
#include <cstring>
#include "aruna/driver/port/cross_platform/FT_I2C.h"

using namespace aruna;
using namespace aruna::driver;
SCENARIO("Using FTDI chip to write I2C commands") {
    GIVEN("MPSSE object for FT2232H") {
        mpsse_context *myMpsseObject = MPSSE(I2C, ONE_HUNDRED_KHZ, MSB);
        FT_I2C FT(myMpsseObject);
        THEN("startup should be fine") {
            REQUIRE(FT.startup_error == err_t::OK);
            REQUIRE(myMpsseObject->vid > 0);
            REQUIRE(strlen(myMpsseObject->description) > 0);
            REQUIRE(myMpsseObject->pid > 0);
        }
        std::cout << "mpsse: " << myMpsseObject->description << " VID " << myMpsseObject->vid << " PID " << myMpsseObject->pid << std::endl;
        WHEN("writing to an I2C address with no slave") {
            err_t err = FT.write(0x10, 0x11, 0x12);
            THEN("there shouldn't be a response") {
                REQUIRE(err_to_char.at(err) == err_to_char.at(err_t::NO_RESPONSE));
            }
        }
        WHEN("reading an I2C address") {
            // TODO
        }
    }
}

#endif //ARUNA_ENABLE_FTDI