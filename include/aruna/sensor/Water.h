//
// Created by noeel on 16-04-20.
//

#ifndef ARUNA_WATER_H
#define ARUNA_WATER_H

#include <aruna/arunaTypes.h>
#include "aruna/sis/Performer.h"

namespace aruna {
    namespace sensor {

// TODO water_status_t is polluting sensor namespace
        struct water_status_t : public sis::status_t {
//            location of sensor
//          TODO location as parameter in constructor.
            char location[7] = "NOTDEV";

            uint8_t water_level_mm = 0;

            /**
             * updates description with water level.
             * @param water_level, uint_8 current water level
             */
            void update_description(uint8_t water_level);

            void encode(uint8_t *buffer) override;

            void decode(uint8_t *to_decode) override;

            uint8_t get_encode_size() override;

            /**
             * water-specific SIS status
             */
            water_status_t();
        };

        class Water : public sis::Performer {
        private:
            water_status_t status = *new water_status_t();

        public:

            /**
             * Water sensor
             */
            Water();

            /**
             * Get water level in millimeters of the sensor
             * @param water_level_in_mm: buffer to store water level in
             * @return err_t::OK if water level could be retrieved
             */
            virtual err_t get_water_level(uint16_t &water_level_in_mm) = 0;

            /**
             * Is water detected at the sensor?
             * @param water_detected: store result
             * @return error retrieving wetness of sensor
             */
            err_t is_wet(bool &water_detected);

            sis::status_t *update_status() override;

            /**
             * Set the location of the sensor. Usefull for SIS status reports.
             * @param location: 7 char long location.
             */
            void set_sis_status_location(char* location);
        };

    }
}


#endif //ARUNA_WATER_H
