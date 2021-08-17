//
// Created by noeel on 19-04-21.
//

#ifndef ARUNA_FT_I2C_H
#define ARUNA_FT_I2C_H
#ifdef ARUNA_ENABLE_FTDI
#include <stddef.h>
#include <mpsse.h>
#include "aruna/log.h"
#include "aruna/driver/I2C_master.h"

namespace aruna::driver {
    class FT_I2C : public I2C_master {
    private:
        static log::channel_t log;
        mpsse_context* mpsse;

        aruna::err_t _write(uint8_t address, uint8_t reg, uint8_t *data, size_t data_size) override;

        aruna::err_t _read(uint8_t address, uint8_t reg, uint8_t *buffer, size_t buffer_size) override;
    public:

        err_t startup_error = err_t::NOT_STARTED;
        FT_I2C(struct mpsse_context * mpsse);
        ~FT_I2C();

    };
}

#endif //ARUNA_ENABLE_FTDI
#endif //ARUNA_FT_I2C_H
