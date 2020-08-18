//
// Created by noeel on 17-08-20.
//
#include "aruna/driver/ADS101x/ADS101x.h"

namespace aruna {
    namespace driver {
        namespace ADS101x {
            uint16_t read(MUX compare, uint8_t address) {
                uint16_t VCC_mV = 3300;
                int16_t conversion = 0;
                uint16_t config = (int) OS::W_START_A_SINGLE_CONVERSION << 15 |
                                  (int) compare << 12 |
                                  (int) PGA::DEFAULT << 9 |
                                  (int) MODE::CONTINUOUS_CONVERSION << 8 |
                                  (int) DR::DEFAULT << 5 |
                                  (int) COMP_MODE::DEFAULT << 4 |
                                  (int) COMP_POL::DEFAULT << 3 |
                                  (int) COMP_LAT::DEFAULT << 2 |
                                  (int) COMP_QUEUE::DEFAULT;
                conversion = readConversion(config, address);
                conversion = conversion >> 4;
//              TODO use function from Actuator.h
                return ((conversion * (VCC_mV - 0)) / 2047) + 0;

            }
        }
    }
}