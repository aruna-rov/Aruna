//
// Created by noeel on 27-04-21.
//

#ifndef ARUNA_MOCKADC_H
#define ARUNA_MOCKADC_H

#include "aruna/driver/ADC.h"
#include <gmock/gmock.h>

class MockADC : public aruna::driver::ADC {
public:
    MOCK_METHOD(aruna::err_t, _read, (int32_t & raw));

    MockADC(uint16_t reference_Mv, size_t resolution_bits) {
        set_reference(reference_Mv);
        set_resolution(resolution_bits);
    }
};


#endif //ARUNA_MOCKADC_H
