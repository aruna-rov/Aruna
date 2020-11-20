//
// Created by noeel on 16-11-20.
//

#include "aruna/driver/UART.h"

using namespace aruna;
using namespace aruna::driver;

size_t UART::write(uint8_t *data, size_t dataSize) {
    size_t ret = 0;
    pthread_mutex_lock(&write_atomic);
    ret = _write(data, dataSize);
    pthread_mutex_unlock(&write_atomic);
    return ret;
}

size_t UART::read(uint8_t *buffer, size_t length) {
    size_t ret = 0;
    pthread_mutex_lock(&read_atomic);
    ret = _read(buffer, length);
    pthread_mutex_unlock(&read_atomic);
    return ret;
}

uint32_t UART::set_baudrate(uint32_t new_baudrate) {
    uint32_t ret = 0;
    ret = _set_baudrate(new_baudrate);
    this->baudrate = ret == new_baudrate ? new_baudrate : baudrate;
    return ret;
}

uint32_t UART::get_baudrate() {
    return baudrate;
}

err_t UART::set_flowcontrol(UART::flowcontrol_t new_flowcontrol) {
    err_t ret = err_t::OK;
    ret = _set_flowcontrol(new_flowcontrol);
    this->flowcontrol = ret != err_t::OK ? this->flowcontrol : new_flowcontrol;
    return ret;
}

UART::flowcontrol_t UART::get_flowcontrol() {
    return flowcontrol;
}

err_t UART::set_parity(UART::parity_t parity_bit) {
    err_t ret = err_t::OK;
    ret = _set_parity(parity_bit);
    this->parity = ret != err_t::OK ? this->parity : parity_bit;
    return ret;
}

UART::parity_t UART::get_parity() {
    return parity;
}

err_t UART::set_stop_bit(UART::stop_bit_t stop_bit) {
    err_t ret = err_t::OK;
    ret = _set_stop_bit(stop_bit);
    this->stopBit = ret != err_t::OK ? this->stopBit : stop_bit;
    return ret;
}

UART::stop_bit_t UART::get_stop_bit() {
    return stopBit;
}

UART::UART() {
//    write
    pthread_mutexattr_t write_attr;
    pthread_mutexattr_init(&write_attr);
    pthread_mutex_init(&write_atomic, &write_attr);
    pthread_mutexattr_destroy(&write_attr);
//   read
    pthread_mutexattr_t read_attr;
    pthread_mutexattr_init(&read_attr);
    pthread_mutex_init(&read_atomic, &read_attr);
    pthread_mutexattr_destroy(&read_attr);
}

UART::~UART() {
    pthread_mutex_destroy(&write_atomic);
    pthread_mutex_destroy(&read_atomic);
}

size_t UART::try_write(uint8_t *data, size_t dataSize) {
    size_t ret = 0;
    if (!pthread_mutex_trylock(&write_atomic)) {
        ret = _write(data, dataSize);
        pthread_mutex_unlock(&write_atomic);
    }
    return ret;
}

size_t UART::try_read(uint8_t *buffer, size_t length) {
    size_t ret = 0;
    if (!pthread_mutex_trylock(&read_atomic)) {
        ret = _read(buffer, length);
        pthread_mutex_unlock(&read_atomic);
    }
    return ret;
}
