//
// Created by noeel on 19-04-21.
//

#include <malloc.h>
#include <cstring>
#include "aruna/driver/port/cross_platform/FT_I2C.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace aruna;
using namespace aruna::log;
using namespace aruna::driver;
const static size_t ACK_TIME = 0;

log::channel_t FT_I2C::log("FT_I2C");

err_t FT_I2C::_write(uint8_t address, uint8_t reg, uint8_t *data, size_t data_size) {
    size_t write_size = 2 + data_size; // address + reg + datasize
    err_t ret = err_t::HARDWARE_FAILURE;
    char *writebuffer = (char *) malloc(write_size);
    writebuffer[0] = ((address << 1) | 0b00000000);
    writebuffer[1] = reg;
    for (int i = 0; i < data_size; ++i) {
        writebuffer[i + 2] = data[i];
    }
    if (Start(mpsse) == MPSSE_OK) {
        if (Write(mpsse, writebuffer, write_size) == MPSSE_OK) {
            if (GetAck(mpsse) == ACK)
                ret = err_t::OK;
            else
                ret = err_t::NO_RESPONSE;
        }
    }
    if (Stop(mpsse) != MPSSE_OK)
        ret = err_t::HARDWARE_FAILURE;
    free(writebuffer);
    return ret;
}

err_t FT_I2C::_read(uint8_t address, uint8_t reg, uint8_t *buffer, size_t buffer_size) {

    err_t ret = err_t::HARDWARE_FAILURE;
    const static size_t write_size = 2; // address + reg
    char write_buffer[write_size];
    uint8_t *read_buffer = (uint8_t *) malloc(buffer_size);
    write_buffer[0] = ((address << 1) | 0b00000000); // write mode
    write_buffer[1] = reg;
    if (Start(mpsse) == MPSSE_OK) {
        if (Write(mpsse, write_buffer, write_size) == MPSSE_OK) {
            if (GetAck(mpsse) == ACK) {
                if (Start(mpsse) == MPSSE_OK) {
                    char w = ((address << 1) | 0b00000001); // read mode
                    if (Write(mpsse, &w, 1) == MPSSE_OK) {
                        if (GetAck(mpsse) == ACK) {
                            read_buffer = (uint8_t *)Read(mpsse, buffer_size);
                            memcpy(buffer, read_buffer, buffer_size);
                            SendNacks(mpsse);
                            ret = err_t::OK;
                        } else {
                            ret = err_t::NO_RESPONSE;
                        }
                    }
                }
            } else {
                ret = err_t::NO_RESPONSE;
            }
        }
    }
    if (Stop(mpsse) != MPSSE_OK)
        ret = err_t::HARDWARE_FAILURE;
    free(read_buffer);
    return ret;
}

FT_I2C::FT_I2C(struct mpsse_context *mpsse) {
    if (mpsse != NULL && !mpsse->open) {
        log.error("Failed to initialize MPSSE: %s", ErrorString(mpsse));
        startup_error = err_t::HARDWARE_FAILURE;
    } else {
        startup_error = err_t::OK;
    }
    this->mpsse = mpsse;
}

FT_I2C::~FT_I2C() {
    // TODO mpsse word niet in deze klasse gemaakt, dus hij mag hem ook niet afsluiten. Maar anders werken mijn unittests niet.
    Close(mpsse);
}
