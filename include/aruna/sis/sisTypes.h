//
// Created by noeel on 15-04-20.
//

#ifndef ARUNA_SISTYPES_H
#define ARUNA_SISTYPES_H

#include <cstddef>
#include <cstring>

namespace aruna {
    namespace sis {
        /**
         * SIS urgency level
         */
        enum class level_t : uint8_t {
            NOTIFY = 0,
            WARNING = 1,
            CRITICAL = 2,
            MAX = 3,
        };

        /**
         * Type of status
         */
        enum class type_t : uint8_t {
            UNKNOWN = 0,
            DEFAULT = 1,
            WATER = 2,
            MAX = 3,
        };

        /**
         * status that is to be reported to the watcher
         */
        struct status_t {
//          TODO constructor and encode_size const?

            level_t level = level_t::NOTIFY;
            char description[32];
            type_t type;

            explicit status_t(type_t type): type(type) {};
            virtual uint8_t get_encode_size() {
                return strlen(description) + 3;
            }

            virtual void encode(uint8_t *buffer) {
                buffer[0] = (uint8_t) type;
                buffer[1] = (uint8_t) level;
                buffer[2] = strlen(description);
                memcpy((char *) &buffer[3], (const char *) description, strlen(description));
            }

            virtual void decode(uint8_t *to_decode) {
                type = (type_t) to_decode[0];
                level = (level_t) to_decode[1];
                memcpy(description, (char *) &to_decode[3], to_decode[2]);
            }
        };
    }

}
#endif //ARUNA_SISTYPES_H
