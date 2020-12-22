//
// Created by noeel on 18-08-20.
//

#include "aruna/sis/ADS101xWaterSensor.h"
#include "math.h"
using aruna::driver::ADS101x;

aruna::sis::ADS101xWaterSensor::ADS101xWaterSensor(ADS101x::MUX compare, uint8_t I2C_address, pthread_mutex_t *I2C_mutex,
                                                   char location[7]) : compare(compare), I2C_address(I2C_address),
                                                                     I2C_mutex(I2C_mutex) {
    log = new log::channel_t("ADS101xWaterSensor");
//    TODO implement this shizzle my dizzle.
//    if (!isConnected(I2C_address))
//        log->error("I2C device: %i is not connected", I2C_address);
    strcpy(status.location, location);
    status.water_level_mm = 0;
    status.level = level_t::NOTIFY;
    status.update_description(0);
    set_update(true);
}

int16_t aruna::sis::ADS101xWaterSensor::voltage_to_mm(uint16_t mV) {
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
//    TODO make a global voltage_to_mm function because this is just copied from esp32/WaterSensor.cpp
    return ceilf(((float) mV - (float) voltage_at_min) * ((float) sensor_length_mm / (float) voltage_diff));

}

aruna::sis::status_t *aruna::sis::ADS101xWaterSensor::update_status() {
    uint16_t mV = 0;
    uint16_t mm;
    pthread_mutex_lock(I2C_mutex);
//    mV = read(compare, I2C_address);
    pthread_mutex_unlock(I2C_mutex);
    mm = voltage_to_mm(mV);
    status.water_level_mm = mm;
    status.level = mm ? level_t::CRITICAL : level_t::NOTIFY;
    status.update_description(mm);
    log->debug("%i mm, %i mV", mm, mV);
    return &status;
}

aruna::err_t aruna::sis::ADS101xWaterSensor::get_water_level(uint16_t &water_level_in_mm) {
//    TODO implement
    return aruna::err_t::NOT_SUPPORTED;
}
