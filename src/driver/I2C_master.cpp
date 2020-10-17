//
// Created by noeel on 15-10-20.
//

#include "aruna/driver/I2C_master.h"

aruna::err_t aruna::driver::I2C_master::read(uint8_t address, uint8_t reg, uint8_t *buffer, size_t buffer_size) {
    pthread_mutex_lock(&line_busy);
    err_t ret;
    ret = _read(address, reg, buffer, buffer_size);
    pthread_mutex_unlock(&line_busy);
    return ret;
}

aruna::err_t aruna::driver::I2C_master::write(uint8_t address, uint8_t reg, uint8_t *data, size_t data_size) {
    pthread_mutex_lock(&line_busy);
    err_t ret;
    ret = _write(address, reg, data, data_size);
    pthread_mutex_unlock(&line_busy);
    return ret;
}

aruna::driver::I2C_master::I2C_master() {
    pthread_mutexattr_init(&line_busy_attr);
    pthread_mutex_init(&line_busy, &line_busy_attr);
}

aruna::driver::I2C_master::~I2C_master() {
    pthread_mutex_destroy(&line_busy);
    pthread_mutexattr_destroy(&line_busy_attr);
}
