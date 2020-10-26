//
// Created by noeel on 18-10-20.
//

#ifndef ARUNA_PCA9685_H
#define ARUNA_PCA9685_H

#include "aruna/driver/Pwm.h"
#include "aruna/driver/I2C_master.h"
#include "aruna/log.h"
#include "pthread.h"

namespace aruna {
    namespace driver {
        class PCA9685 : public Pwm {
        private:

            static aruna::log::channel_t log;
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

            uint8_t const static default_i2c_address = 0b1000000;
            const uint8_t led, i2c_address;
            I2C_master* i2c_bus;

            err_t _set_frequency(uint32_t frequency_hz) override;

            err_t _set_duty(uint16_t duty) override;
        public:
            /**
             * PCA9685 pwm object for every LED
             * @param led: 0-15 led number
             * @param i2c_address: I²C address of the IC
             * @param i2c_bus: Bus to use for I²C communication.
             */
            PCA9685(uint8_t led, I2C_master *i2c_bus, uint8_t i2c_address = default_i2c_address);
//            TODO supply a default i2c_bus per port in aruna_port.h or something

            ~PCA9685();


//            TODO without override does this function get called?
            uint32_t get_frequency();


            /**
             * Set the on and off timing of the PCA9685 led
             * @param on: 12 bit time to set signal to high
             * @param off: 12 bit time to set signal to low
             * @return I²C bus error
             */
            err_t set_duty(uint16_t on, uint16_t off);

//            TODO without override does this function get called?
            uint16_t get_duty();

            /**
             * Get the on and off timing of the PCA9685 led
             * @param on: 12 bit time to set signal to high
             * @param off: 12 bit time to set signal to low
             * @return I²C bus error
             */
            err_t get_duty(uint16_t &on, uint16_t &off);

            /**
             * Is the PCA9685 connected to the I²C bus.
             * @note only checks if a device with said I²C address returns an ACK.
             * @return I²C error code.
             *  - err_t::OK success
             *  - err_t::NO_RESPONSE slave doesn't ACK the transfer
             *  - err_t::INVALID_PARAMETERS parameter error
             *  - err_t::HARDWARE_FAILURE I2C driver malfunction
             */
            err_t is_connected();

//            TODO support for SUBADRx
//            TODO support for ALLCALLADR
//            TODO support ALL_LED write
//            TODO support write/read mode1/2 directly
        };
    }
}


#endif //ARUNA_PCA9685_H
