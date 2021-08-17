/*
 * ADC.cpp
 *
 *  Created on: 30 nov. 2020
 *      Author: noeel
 */

#include "aruna/driver/ADC.h"

namespace aruna {
    namespace driver {

        err_t ADC::set_resolution(size_t bits) {
            resolution = bits;
            return err_t::OK;
        }

        err_t ADC::set_reference(uint16_t mV) {
            reference = mV;
            return err_t::OK;
        }

        size_t ADC::get_resolution() const {
            return resolution;
        }

        int32_t ADC::raw_to_voltage(int32_t raw) {
            double input, inputLow, inputHigh, outputHigh, outputLow;
            input = raw;
            inputLow = 0;
            inputHigh = INT32_MAX;
            outputHigh = reference;
            outputLow = 0;
// TODO make convert_range function and replace Actuator::convert_range with this
// https://gamedev.stackexchange.com/questions/33441/how-to-convert-a-number-from-one-min-max-set-to-another-min-max-set
            return ((input - inputLow) / (inputHigh - inputLow)) * (outputHigh - outputLow) + outputLow;

        }

        uint16_t ADC::get_reference() const {
            return reference;
        }

    }
}