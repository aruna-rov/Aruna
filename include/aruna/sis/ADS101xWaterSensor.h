//
// Created by noeel on 18-08-20.
//

#ifndef ARUNA_ADS101XWATERSENSOR_H
#define ARUNA_ADS101XWATERSENSOR_H

#include "aruna/sis/Water.h"
#include "aruna/log.h"
#include "pthread.h"
#include "aruna/driver/ADS101x.h"
namespace aruna {
    namespace sis {
        class ADS101xWaterSensor: public Water {
        private:
            int16_t voltage_to_mm(uint16_t mV);
            const driver::ADS101x::MUX compare;
            const uint8_t I2C_address;
            pthread_mutex_t* I2C_mutex;
            log::channel_t* log;
        public:

            /**
             * WaterSensor using ADS101x ADC.
             * @param compare, compare pin AINp with AINn
             * @param I2C_address, address of ADS101x
             * @param I2C_mutex, to keep I²C thead safe, init the mutex before hand.
             * @param location , location of the sensor.
             */
            ADS101xWaterSensor(driver::ADS101x::MUX compare, uint8_t I2C_address, pthread_mutex_t *I2C_mutex,
                               char location[7]);

            status_t* update_status() override;
        };
    }
}


#endif //ARUNA_ADS101XWATERSENSOR_H
