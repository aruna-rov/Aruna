//
// Created by noeel on 16-04-20.
//

#include <cstdio>
#include "aruna/sensor/Water.h"

using namespace aruna::sis;
using namespace aruna::sensor;

Water::Water() : status() {
    status.water_level_mm = 0;
    status.level = level_t::NOTIFY;
//    update description
    status.update_description(status.water_level_mm);
}

aruna::err_t Water::is_wet(bool &water_detected) {
    err_t e;
    uint16_t i;
    e = get_water_level(i);
    water_detected = i != 0;
    return e;
}

status_t *Water::update_status() {
    uint16_t mm;
    err_t e;
    e = get_water_level(mm);
    if ((uint8_t) e)
        log->error("Error retrieving water level: %s", err_to_char.at(e));
    status.water_level_mm = mm;
    status.level = mm ? level_t::CRITICAL : level_t::NOTIFY;
    status.update_description(mm);
    return &status;
}

void Water::set_sis_status_location(char *location) {
    strcpy(status.location, location);
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
