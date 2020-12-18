/*
 * ADC.cpp
 *
 *  Created on: 30 nov. 2020
 *      Author: noeel
 */

#ifndef ARUNA_ADC_H
#error Include ADC.h
#endif

#ifndef ARUNA_ADC_IMP_H
#define ARUNA_ADC_IMP_H

template<typename T>
aruna::err_t aruna::driver::ADC::read_raw(T &raw) {
    return _read(&raw);
}

template<typename T>
aruna::err_t aruna::driver::ADC::read_voltage(T &mV) {
    int32_t raw;
    aruna::err_t err;
    err = read_raw(raw);
    if ((bool) err)
        return err;

    mV = raw_to_voltage(raw);
    return aruna::err_t::OK;
}


#endif /* ARUNA_ADC_IMP_H */