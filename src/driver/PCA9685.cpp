//
// Created by noeel on 18-10-20.
//

#include <cmath>
#include "aruna/driver/PCA9685.h"

using namespace aruna;
using namespace aruna::driver;

log::channel_t PCA9685::log("PCA9685");

PCA9685::PCA9685(uint8_t led, I2C_master *i2c_bus, uint8_t i2c_address) : led(led), i2c_address(i2c_address) {
    this->i2c_bus = i2c_bus;

//    read device
    err_t i2c_err;
    uint8_t mode1;
    i2c_bus->lock(i2c_address);
    i2c_err = i2c_bus->read(i2c_address, static_cast<uint8_t>(register_address_pointer::MODE1), &mode1, 1);
    if (i2c_err != err_t::OK)
        log.error("Error while reading MODE1 0x%X: %s", i2c_address, err_to_char.at(i2c_err));

//   check to see if device is powered on
    if (mode1 & (uint8_t) SLEEP::LOW_POWER) {
        i2c_err = i2c_bus->write(i2c_address, (uint8_t) register_address_pointer::MODE1,
                                 reinterpret_cast<uint8_t *>(mode1 | (uint8_t) SLEEP::NORMAL_MODE), 1);
        if (i2c_err != err_t::OK)
            log.error("Error while writing MODE1 0x%X: %s", i2c_address, err_to_char.at(i2c_err));
    }
    i2c_bus->unlock(i2c_address);
}

err_t PCA9685::_set_frequency(uint32_t frequency_hz) {
    if (frequency_hz > 1526)
        return err_t::BUFFER_OVERFLOW;
    if (frequency_hz < 25)
        return err_t::BUFFER_UNDERFLOW;
    err_t i2c_err;
    uint8_t mode1;
//    only works if using internal clock
    uint8_t prescale = round(25000000 / (4096 * frequency_hz)) - 1;
    i2c_bus->lock(i2c_address);
    i2c_err = i2c_bus->read(i2c_address, static_cast<uint8_t>(register_address_pointer::MODE1), &mode1, 1);
    if (i2c_err != err_t::OK)
        log.error("Error while reading MODE1 0x%X: %s", i2c_address, err_to_char.at(i2c_err));
    i2c_err = i2c_bus->write(i2c_address, (uint8_t) register_address_pointer::MODE1,
                             reinterpret_cast<uint8_t *>(mode1 ^ (uint8_t) SLEEP::NORMAL_MODE), 1);
    if (i2c_err != err_t::OK)
        log.error("Error while writing MODE1 0x%X: %s", i2c_address, err_to_char.at(i2c_err));
    i2c_err = i2c_bus->write(i2c_address, (uint8_t) register_address_pointer::PRE_SCALE, &prescale, 1);
    if (i2c_err != err_t::OK)
        log.error("Error while writing PRE_SCALE 0x%X: %s", i2c_address, err_to_char.at(i2c_err));
    i2c_err = i2c_bus->write(i2c_address, (uint8_t) register_address_pointer::MODE1, &mode1, 1);
    i2c_bus->unlock(i2c_address);

    return i2c_err;
}

uint32_t PCA9685::get_frequency() {
    uint8_t prescale = 0;
    err_t i2c_err;
    uint32_t freq = 0;
    i2c_err = i2c_bus->read(i2c_address, (uint8_t) register_address_pointer::PRE_SCALE, &prescale, 1);
    if (i2c_err != err_t::OK)
        log.error("Error while reading PRE_SCALE 0x%X: %s", i2c_address, err_to_char.at(i2c_err));
    freq = round(25000000 / (4096 * prescale)) - 1;
    return freq;
}

err_t PCA9685::_set_duty(uint16_t duty) {

    duty = round(duty / 65535 * 4095);
    uint16_t on = 0;
    uint16_t off = duty;
    return set_duty(on, off);
}

err_t PCA9685::set_duty(uint16_t on, uint16_t off) {
    uint8_t reg = (uint8_t) register_address_pointer::LED0_ON_L + 4 * led;
    uint8_t data[4];
    data[0] = on;
    data[1] = on >> 8;
    data[2] = off;
    data[3] = off >> 8;
    return i2c_bus->write(i2c_address, reg, data, 4);
}

uint16_t PCA9685::get_duty() {
    uint16_t on, off;
    err_t err = get_duty(on, off);
    if (err != err_t::OK)
        log.error("Error while getting duty cycle 0x%X: %s", i2c_address, err_to_char.at(err));
//    TODO test conversion
    uint16_t duty = round(off / 4095 * 65535);
    return duty;
}

err_t PCA9685::get_duty(uint16_t &on, uint16_t &off) {
    uint8_t reg = (uint8_t) register_address_pointer::LED0_ON_L + 4 * led;
    uint8_t buff[4];
    err_t i2c_err = i2c_bus->read(i2c_address, reg, buff, 4);
//    todo test this conversion
    on = buff[0] | buff[1] >> 8;
    off = buff[2] | buff[3] >> 8;
    return i2c_err;
}

PCA9685::~PCA9685() {
    Pwm::set_duty((uint16_t)0);
//    TODO put PCA9685 to sleep if I'm the last one connected to it.
}

err_t PCA9685::is_connected() {
    uint8_t fake_buffer;
    return i2c_bus->read(i2c_address, 0, fake_buffer);
}
