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
//            TODO are negative values possible?
            return ((raw * (this->reference - 0)) / (sizeof(raw) * 8)) + 0;
        }

        uint16_t ADC::get_reference() const {
            return reference;
        }

    }
}