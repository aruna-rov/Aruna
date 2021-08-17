//
// Created by noeel on 17-08-20.
//
#include <math.h>
#include <cstdio>
#include "aruna/driver/ADS101x.h"

using namespace aruna;
using namespace aruna::driver;


err_t ADS101x::_read(int32_t &raw) {
    err_t err = err_t::OK;
    int16_t conversion = 0;
    uint16_t config = (int) ADS101x::OS::W_START_A_SINGLE_CONVERSION |
                      (int) compare |
                      (int) ADS101x::PGA::DEFAULT |
                      (int) ADS101x::MODE::CONTINUOUS_CONVERSION |
                      (int) ADS101x::DR::DEFAULT |
                      (int) ADS101x::COMP_MODE::DEFAULT |
                      (int) ADS101x::COMP_POL::DEFAULT |
                      (int) ADS101x::COMP_LAT::DEFAULT |
                      (int) ADS101x::COMP_QUEUE::DEFAULT;

    uint8_t data[2];
    uint8_t config_array[2]={ static_cast<uint8_t>(config & 0xff), static_cast<uint8_t>(config >> 8) };
//                write config
    err = i2c_bus->write(i2c_address, (uint8_t) register_address_pointer::CONFIG, config_array, 2);
    if ((bool) err)
        return err;
//                set Conversion register to read and read conversion register
    err = i2c_bus->read(i2c_address, (uint8_t) register_address_pointer::CONVERSION, data, 2);
//    printf("read: 0x%X%X\n", data[0], data[1]);
    conversion = (int16_t) (data[0] << 8 | (data[1] & 0xFF));
    conversion = conversion >> 4;
//              TODO use function from Actuator.h
//    raw = ((conversion * (4096 - 0)) / 2047) + 0;

    double input, inputLow, inputHigh, outputHigh, outputLow;
    input = conversion;
    inputLow = -2048;
    inputHigh = 2047;
    outputHigh = INT32_MAX;
    outputLow = INT32_MIN;
// TODO make convert_range function and replace Actuator::convert_range with this
// TODO range converting should be done by the ADC parent.
// https://gamedev.stackexchange.com/questions/33441/how-to-convert-a-number-from-one-min-max-set-to-another-min-max-set
    raw = ((input - inputLow) / (inputHigh - inputLow)) * (outputHigh - outputLow) + outputLow;
    return err;
}


ADS101x::ADS101x(ADS101x::MUX compare, I2C_master *i2c_bus, uint8_t address) : compare(compare), i2c_bus(i2c_bus),
                                                                               i2c_address(address) {
    set_reference(3300);
    set_resolution(12);

}
