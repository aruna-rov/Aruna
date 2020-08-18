//
// Created by noeel on 16-04-20.
//

#include <cstdio>
#include "aruna/sis/Water.h"

using namespace aruna::sis;

Water::Water() : status() {
//    update description
    status.update_description(status.water_level_mm);
}


void water_status_t::update_description(uint8_t water_level) {
    snprintf(description, sizeof(description), "%imm water in %s", water_level, location);
}

void water_status_t::encode(uint8_t *buffer) {
//    standard encoding
    status_t::encode(buffer);
    uint8_t start_byte = status_t::get_encode_size();
    buffer[start_byte] = water_level_mm;
    buffer[start_byte + 1] = strlen(location);
    memcpy((char *) &buffer[start_byte + 2], location, strlen(location));
}

void water_status_t::decode(uint8_t *to_decode) {
    status_t::decode(to_decode);
    uint8_t start_byte = status_t::get_encode_size();
    water_level_mm = to_decode[start_byte];
//    TODO decoding of location is not always correct.
    memcpy(location, (char *) &to_decode[start_byte + 2], to_decode[start_byte + 1]);
}

uint8_t water_status_t::get_encode_size() {
    return status_t::get_encode_size() + strlen(location) + 2;
}

water_status_t::water_status_t() : status_t(type_t::WATER) {
}
