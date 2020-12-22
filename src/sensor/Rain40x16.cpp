//
// Created by noeel on 22-12-20.
//

#include <math.h>
#include "aruna/sensor/Rain40x16.h"

aruna::sensor::Rain40x16::Rain40x16(aruna::driver::ADC *adc): adc(adc) {

}

aruna::err_t aruna::sensor::Rain40x16::get_water_level(uint16_t &water_level_in_mm) {
    int16_t mV;
    err_t e;
    e = adc->read_voltage(mV);
    water_level_in_mm = voltage_to_mm(mV);
    return e;
}

int16_t aruna::sensor::Rain40x16::voltage_to_mm(uint16_t mV) {
//    TODO vcc etc as parameters
    const int16_t sensor_length_mm = 40;
    const int16_t max_voltage_when_dry = 20;
    const int16_t voltage_at_min = 1000;
    const int16_t voltage_at_max = 3300;

    const int16_t voltage_diff = voltage_at_max - voltage_at_min;

    if (mV < max_voltage_when_dry)
        return 0;
    if (mV < voltage_at_min) {
//        voltage is in grey space.
        return 1;
    }
//    TODO this conversion is linear while in real-life it is not
    return ceilf(((float) mV - (float) voltage_at_min) * ((float) sensor_length_mm / (float) voltage_diff));

}