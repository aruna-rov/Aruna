//
// Created by noeel on 14-09-20.
//

#ifndef ARUNA_PCA9685_MAP_H
#define ARUNA_PCA9685_MAP_H

#include "stdint.h"

namespace aruna {
    namespace driver {
        namespace PCA9685 {
            uint8_t const static default_address = 0b1000000;

            /**
             * Pointer to the registers.
             */
            enum class register_address_pointer {
                MODE1 = 0x0,
                MODE2 = 0x1,
                SUBADR1 = 0x2,
                SUBADR2 = 0x3,
                SUBADR3 = 0x4,
                ALLCALLADR = 0x5,
                LED0_ON_L = 0x6,

                ALL_LED_ON_L = 0xFA,
                PRE_SCALE = 0xFE,
                TESTMODE = 0xFF,

            };

//            Mode 1
            enum class RESTART {
                DEFAULT = 0x0,
                DISABLED = 0x0,
                ENABLED = 0x80,
            };

            enum class EXTCLK {
                DEFAULT = 0x0,
                USE_INTERNAL_CLOCK = 0x0,
                USE_EXTCLK_PIN_CLOCK = 0x40,
            };

            enum class AI {
                DEFAULT = 0x0,
                AUTO_INCREMENT_DISABLED = 0x0,
                AUTO_INCREMENT_ENABLED = 0x20,
            };

            enum class SLEEP {
                DEFAULT = 0x10,
                NORMAL_MODE = 0x0,
                LOW_POWER = 0x10,
            };

            enum class SUB1 {
                DEFAULT = 0x0,
                NOT_RESPOND_TO_SUBADDRESS1 = 0x0,
                RESPOND_TO_SUBADDRESS1 = 0x8,
            };

            enum class SUB2 {
                DEFAULT = 0x0,
                NOT_RESPOND_TO_SUBADDRESS2 = 0x0,
                RESPOND_TO_SUBADDRESS2 = 0x4,
            };

            enum class SUB3 {
                DEFAULT = 0x0,
                NOT_RESPOND_TO_SUBADDRESS3 = 0x0,
                RESPOND_TO_SUBADDRESS3 = 0x2,
            };

            enum class ALLCALL {
                DEFAULT = 0x1,
                NOT_RESPOND_TO_LED_ALL = 0x0,
                RESPOND_TO_LED_ALL = 0x1,
            };

//            Mode 2
            enum class INVRT {
                DEFAULT = 0x0,
                OUTPUT_LOGIC_NOT_INVERTED = 0x0,
                OUTPUT_LOGIC_INVERTED = 0x10,
            };

            enum class OCH {
                DEFAULT = 0x0,
                OUTPUT_CHANGE_ON_STOP_COMMAND = 0x0,
                OUTPUT_CHANGE_ON_ACK = 0x8,
            };

            enum class OUTDRV {
                DEFAULT = 0x4,
                OPEN_DRAIN = 0x0,
                TOTEM_POLE = 0x4,
            };

            enum class OUTNE {
                DEFAULT = 0x0,
                LEDn_0 = 0x0,
                LEDn_1_OUTDRV_1 = 0b01,
                LEDn_HIGH_IMPEDANCE = 0b10,
            };
        }
    }
}
#endif //ARUNA_PCA9685_MAP_H
