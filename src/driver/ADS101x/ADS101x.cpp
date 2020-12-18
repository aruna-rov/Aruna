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
                uint16_t config = (int) OS::W_START_A_SINGLE_CONVERSION |
                                  (int) compare |
                                  (int) PGA::DEFAULT |
                                  (int) MODE::CONTINUOUS_CONVERSION |
                                  (int) DR::DEFAULT |
                                  (int) COMP_MODE::DEFAULT |
                                  (int) COMP_POL::DEFAULT |
                                  (int) COMP_LAT::DEFAULT |
                                  (int) COMP_QUEUE::DEFAULT;
                conversion = readConversion(config, address);
                conversion = conversion >> 4;
//              TODO use function from Actuator.h
                return ((conversion * (VCC_mV - 0)) / 2047) + 0;

            }
        }
    }
}