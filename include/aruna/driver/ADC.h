/*
 * ADC.h
 *
 *  Created on: 30 nov. 2020
 *      Author: noeel
 */

#ifndef ARUNA_ADC_H
#define ARUNA_ADC_H

#include "aruna/arunaTypes.h"

namespace aruna {
    namespace driver {

        /**
         * Analogue digital converter
         */
        class ADC {
        private:
            /**
             * resolution of the ADC in bits.
             */
            size_t resolution = 0;

            /**
             * ADC voltage refrence, for convertage to voltage.
             */
            uint16_t reference = 0;

            /**
             * Read analogue value and convert to 32bit signed resolution
             * @param raw: store result here
             * @return error code
             */
            virtual err_t _read(int32_t &raw) = 0;

        protected:
            /**
             * Set resolution of ADC. All logic still uses 16bit, this is only for reference
             * @param bits: new resolution
             * @return err_t::OK
             */
            err_t set_resolution(size_t bits);

            /**
             * set reference voltage, used to convert raw data to voltage.
             * @param mV: micro Voltage
             * @return err_t::OK
             */
            err_t set_reference(uint16_t mV);

        public:

            /**
             * Read raw value from ADC, convert to voltage yourself
             * @param raw: stores result
             * @return errors
             */
            template<typename T>
            err_t read_raw(T &raw);

            /**
             * Read voltage level of input
             * @param mV: int stores microvoltage
             * @return errors
             */
            template<typename T>
            err_t read_voltage(T &mV);

            /**
             * Get the resolution in bits of the ADC.
             * Use this value to pick the right int for the template functions
             * @return resolution of this ADC
             */
            size_t get_resolution() const;

            /**
             * Get the reference voltage in micro Voltage.
             * @return micro Voltage, used by the ADC as reference.
             */
            uint16_t get_reference() const;

            /**
             * Convert raw data to voltage.
             * @param raw data from ADC
             * @return voltage in mV.
             */
            virtual int32_t raw_to_voltage(int32_t raw);
        };

    } /* namespace driver */
} /* namespace aruna */

#include "ADC.hpp"

#endif /* ARUNA_ADC_H */
