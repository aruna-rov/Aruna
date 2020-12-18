//
// Created by noeel on 17-08-20.
//

#ifndef ARUNA_ADS101X_MAP_H
#define ARUNA_ADS101X_MAP_H

#include "stdint.h"
namespace aruna {
    namespace driver {
        namespace ADS101x {
            uint8_t const static default_address = 0b1001000;

            /**
             * Pointer to the registers.
             */
            enum class register_address_pointer {
                CONVERSION = 0b00,
                CONFIG = 0b01,
                LO_TRESH = 0b10,
                HI_TRESH = 0b11,
            };

            /**
             * write these values to the respected registers to reset them to their default value.
             */
            enum class reset_value {
                CONVERSION = 0x0000,
                CONFIG = 0x8583,
                LO_THRESH = 0x8000,
                HI_THRESH = 0x7FFF,
            };

            /**
             * Operational status or single-shot conversion start
             * This bit determines the operational status of the device. OS can only be written
             * when in power-down state and has no effect when a conversion is ongoing.
             */
            enum class OS {
                W_NO_EFFECT = 0b0 << 15,
                W_START_A_SINGLE_CONVERSION = 0b1 << 15,
                R_DEVICE_IS_CURRENTLY_PERFORMING_A_CONVERSION = 0b0 << 15,
                R_DEVICE_IS_NOT_CURRENTLY_PERFORMING_A_CONVERSION = 0b1 << 15,
            };

            /**
             * Input multiplexer configuration (ADS1015 only)
             * These bits configure the input multiplexer. These bits serve no function on the ADS1013 and ADS1014.
             */
            enum class MUX {
                DEFAULT = 0b000 << 12,
                AINp0_AINn1 = 0b000 << 12,
                AINp0_AINn3 = 0b001 << 12,
                AINp1_AINn3 = 0b010 << 12,
                AINp2_AINn3 = 0b011 << 12,
                AINp0_AINnGND = 0b100 << 12,
                AINp1_AINnGND = 0b101 << 12,
                AINp2_AINnGND = 0b110 << 12,
                AINp3_AINnGND = 0b111 << 12,
            };

            /**
             * Programmable gain amplifier configuration
             * These bits set the FSR of the programmable gain amplifier. These bits serve no function on the ADS1013.
             */
            enum class PGA {
                DEFAULT = 0b010 << 9,
                FSR_6144mV = 0b000 << 9,
                FSR_4096mV = 0b001 << 9,
                FSR_2048mV = 0b010 << 9,
                FSR_1024mV = 0b011 << 9,
                FSR_0512mV = 0b100 << 9,
                FSR_0256mV = 0b101 << 9,
            };

            /**
             * Device operating mode
             * This bit controls the operating mode.
             */
            enum class MODE {
                DEFAULT = 0b1 << 8,
                CONTINUOUS_CONVERSION = 0b0 << 8,
                SINGLE_SHOT = 0b1 << 8,
            };

            /**
             * Data rate
             * These bits control the data rate setting.
             */
            enum class DR {
                DEFAULT = 0b100 << 5,
                SPS_128 = 0b000 << 5,
                SPS_250 = 0b001 << 5,
                SPS_490 = 0b010 << 5,
                SPS_920 = 0b011 << 5,
                SPS_1600 = 0b100 << 5,
                SPS_2400 = 0b101 << 5,
                SPS_3300 = 0b110 << 5,
            };

            /**
             * Comparator mode (ADS1014 and ADS1015 only)
             * This bit configures the comparator operating mode. This bit serves no function on the ADS1013.
             */
            enum class COMP_MODE {
                DEFAULT = 0b0 << 4,
                TRADITIONAL = 0b0 << 4,
                WINDOW = 0b1 << 4,
            };

            /**
             * Comparator polarity (ADS1014 and ADS1015 only)
             * This bit controls the polarity of the ALERT/RDY pin. This bit serves no function on the ADS1013.
             */
            enum class COMP_POL {
                DEFAULT = 0b0 << 3,
                ACTIVE_LOW = 0b0 << 3,
                ACTIVE_HIGH = 0b1 << 3,
            };

            /**
             * Latching comparator (ADS1014 and ADS1015 only)
             * This bit controls whether the ALERT/RDY pin latches after being asserted or
             * clears after conversions are within the margin of the upper and lower threshold values.
             * This bit serves no function on the ADS1013.
             */
            enum class COMP_LAT {
                DEFAULT = 0b0 << 2,
                NONLATCHING_COMPARATOR = 0b0 << 2,
                LATCHING_COMPARATOR = 0b1 << 2,
            };

            /**
             * Comparator queue and disable (ADS1014 and ADS1015 only)
             * These bits perform two functions. When set to 11, the comparator is disabled and
             * the ALERT/RDY pin is set to a high-impedance state. When set to any other value,
             * the ALERT/RDY pin and the comparator function are enabled, and the set
             * value determines the number of successive conversions exceeding the upper or
             * lower threshold required before asserting the ALERT/RDY pin.
             * These bits serve no function on the ADS1013.
             */
            enum class COMP_QUEUE {
                DEFAULT = 0b11,
                ASSERT_AFTER_ONE_CONVERSION = 0b00,
                ASSERT_AFTER_TWO_CONVERSIONS = 0b01,
                ASSERT_AFTER_FOUR_CONVERSION = 0b10,
                DISABLE_COMPARATOR_AND_SET_ALERT_RDY_PIN_TO_HIGH_IMPEDANCE = 0b11,
            };


        }
    }
}

#endif //ARUNA_ADS101X_MAP_H
