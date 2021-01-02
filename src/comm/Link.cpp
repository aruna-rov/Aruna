//
// Created by noeel on 26-12-20.
//

#include "aruna/comm/Link.h"

namespace aruna::comm {

    size_t Link::transmit(uint8_t *package, uint8_t package_size) {
        return _write(package, package_size);
    }

    size_t Link::receive(uint8_t *buffer, uint8_t buffer_size){
        return _read(buffer, buffer_size);
    }

    bool Link::is_connected() {
//        TODO implement
        return true;
    }
}