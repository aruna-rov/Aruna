//
// Created by noeel on 16-04-20.
//

#ifndef ARUNA_WATERSENSOR_H
#define ARUNA_WATERSENSOR_H

#include "aruna/sis/Water.h"
// TODO this path will not work on other computers
#include "../../../../../../../../../../esp/esp-idf/components/esp_adc_cal/include/esp_adc_cal.h"

namespace aruna {
    namespace sis {
        class WaterSensor: public Water {
        private:
            esp_adc_cal_characteristics_t *adc_chars;
            int16_t voltage_to_mm(uint32_t voltage);
            const adc1_channel_t adc_channel;

        public:
            /**
             * watersensor
             * @param adc_channel, adc1 channel to use
             * @param location, location of the sensor (max 7 byte)
             */
            WaterSensor(adc1_channel_t adc_channel, char location[7]);
            ~WaterSensor();

            status_t * update_status() override;
        };
    }
}


#endif //ARUNA_WATERSENSOR_H
