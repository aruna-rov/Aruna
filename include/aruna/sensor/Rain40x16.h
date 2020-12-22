//
// Created by noeel on 22-12-20.
//

#ifndef ARUNA_RAIN40X16_H
#define ARUNA_RAIN40X16_H

#include "aruna/sensor/Water.h"
#include "aruna/driver/ADC.h"

namespace aruna::sensor {
    class Rain40x16: public Water {
    private:
        driver::ADC* adc;
        int16_t voltage_to_mm(uint16_t mV);
    public:
        /**
         * Rainlevel sensor 40mmx16mm DC3-5V
         * Currently hardcoded for 3.3V
         * @param adc: ADC to use for conversion
         */
        Rain40x16(driver::ADC* adc);

        err_t get_water_level(uint16_t &water_level_in_mm) override;

    };
}



#endif //ARUNA_RAIN40X16_H
