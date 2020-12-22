//
// Created by noeel on 16-04-20.
//

#ifndef ARUNA_WATER_H
#define ARUNA_WATER_H

#include "aruna/sis/Performer.h"

namespace aruna {
    namespace sis {


        struct water_status_t : public status_t {
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

        class Water : public Performer {

        public:
            water_status_t status = *new water_status_t();

            /**
             * Water specific SIS performer
             */
            Water();
        };

    }
}


#endif //ARUNA_WATER_H
