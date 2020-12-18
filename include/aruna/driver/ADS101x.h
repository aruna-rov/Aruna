//
// Created by noeel on 17-08-20.
//

#ifndef ARUNA_ADS101X_H
#define ARUNA_ADS101X_H

#include "aruna/driver/ADS101x/ADS101x_map.h"

namespace aruna {
    namespace driver {
        namespace ADS101x {
            /**
             * Read conversion
             * @param config to write to the chip before reading conversion
             * @param address, I²C address
             * @return conversion in 12bit two compliments
             */
            int16_t readConversion(uint16_t config, uint8_t address = default_address);

            /**
             * Read mV from AINx pin
             * Make sure the I²C bus is initialized, this API is not thread-safe.
             * @param compare, compare AINp to AINn
             * @param address, I²C address of the ADS101x chip
             * @return value of AINp in mV.
             */
            uint16_t read(MUX compare, uint8_t address = default_address);

            /**
             * check if device is connected and functional
             * @param address, I²C address
             * @return true if OK.
             */
            bool isConnected(uint8_t address);
        }
    }
}


#endif //ARUNA_ADS101X_H
