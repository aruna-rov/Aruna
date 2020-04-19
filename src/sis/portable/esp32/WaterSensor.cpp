//
// Created by noeel on 16-04-20.
//

#include "aruna/sis/portable/esp32/WaterSensor.h"
#include "driver/adc.h"
// TODO this path is super weird!
#include "../../../../../../../../../esp/esp-idf/components/esp_adc_cal/include/esp_adc_cal.h"

using namespace aruna::sis;

status_t * WaterSensor::update_status() {
//    TODO multisampling
    uint32_t reading = adc1_get_raw(adc_channel);
    uint32_t voltage = esp_adc_cal_raw_to_voltage(reading, adc_chars);
    int16_t mm = voltage_to_mm(voltage);
    log->debug("mm: %i, voltage: %i.", mm, voltage);
    status.water_level_mm = mm;
    status.level = mm ? level_t::CRITICAL : level_t::NOTIFY;
    status.update_description(mm);
    return &status;
}

WaterSensor::WaterSensor(adc1_channel_t adc_channel, char location[7]): adc_channel(adc_channel) {
//    TODO allow to use adc2
    strcpy(status.location, location);
    status.water_level_mm = 0;
    status.level = level_t::NOTIFY;
    status.update_description(0);
    adc_chars = static_cast<esp_adc_cal_characteristics_t *>(calloc(1, sizeof(esp_adc_cal_characteristics_t)));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, adc_chars);
    log::set_level(log_tag, log::level_t::DEBUG);
    set_update(true);
}

WaterSensor::~WaterSensor() {
// todo close adc
}

int16_t WaterSensor::voltage_to_mm(uint32_t voltage) {
    const int16_t sensor_length_mm = 40;
    const int16_t min_voltage_when_dry = 3000;
    const int16_t voltage_at_min = 1000;
    const int16_t voltage_at_max = 240;

    const int16_t voltage_diff = voltage_at_min - voltage_at_max;

    if (voltage < min_voltage_when_dry && voltage > voltage_at_min) {
//        voltage is in grey space.
        return 1;
    }
//    TODO this decoding is not accurate.
    int16_t mm_water = 0;
    mm_water = voltage_at_min - voltage;
    mm_water *= (float) sensor_length_mm / (float) voltage_diff;
    mm_water = mm_water < 0 ? 0 : mm_water;
    return mm_water;
}