//
// Created by noeel on 26-04-21.
//

#ifndef ARUNA_MOCKI2C_MASTER_H
#define ARUNA_MOCKI2C_MASTER_H

#include "gmock/gmock.h"
class MockI2C_master : public aruna::driver::I2C_master {
public:
    MOCK_METHOD(aruna::err_t, _read, (uint8_t address, uint8_t reg, uint8_t* data, size_t data_size), (override));
    MOCK_METHOD(aruna::err_t, _write, (uint8_t address, uint8_t reg, uint8_t* buffer, size_t buffer_size), (override));
};
#endif //ARUNA_MOCKI2C_MASTER_H
