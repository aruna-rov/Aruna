//
// Created by noeel on 15-04-20.
//

#include "aruna/sis/watcher.h"
#include "aruna/log.h"
#include <pthread.h>
#include <aruna/sis/reporter.h>
#include <aruna/sis/Water.h>

using namespace aruna;
namespace {
    comm::channel_t *comm_channel;
    log::channel_t *log_channel;
    pthread_t watcher_thread;
    sis::level_t level[(uint8_t) sis::type_t::MAX];
}

err_t sis::watcher::start() {
    comm_channel = new comm::channel_t(port);
//    TODO error check
    log_channel = new log::channel_t("SIS watcher");
    for (auto &i : level) {
        i = level_t::WARNING;
    }
    pthread_create(&watcher_thread, NULL, watch, NULL);
    return comm_channel->register_err;
}

void *sis::watcher::watch(void *) {
    comm::transmitpackage_t tp;
    sis::status_t *status_buffer;
    sis::type_t received_type;
    while (1) {
        comm_channel->receive(&tp);
        if (tp.from_port != sis::reporter::port) {
            comm_channel->receive_clear();
            log_channel->warning("Port: %i sending us packages, dropping:", tp.from_port);
            log_channel->dump(log::level_t::WARNING, tp.data_received, tp.data_lenght);
            continue;
        }
        received_type = (sis::type_t) tp.data_received[0];

//        TODO this feels off, is there any better way of handling this?
        switch (received_type) {
            case type_t::WATER:
                status_buffer = new sis::water_status_t();
                break;
            case type_t::UNKNOWN:
            case type_t::MAX:
            default:
                received_type = type_t::UNKNOWN;
            case sis::type_t::DEFAULT:
                status_buffer = new sis::status_t(received_type);
                break;
        }

        status_buffer->decode(tp.data_received);
        if ((uint8_t) level[(uint8_t) received_type] <= (uint8_t) status_buffer->level) {
//            TODO error on critical and warning on warning.
//            TODO better log containing type and level.
//            TODO water type specific handler
            log_channel->error(status_buffer->description);
//            TODO log NOTIFY after >WARNING
        } else {
//            TODO log in verbose
        }
        delete status_buffer;
        comm_channel->receive_clear();
    }
    return nullptr;
}

void sis::watcher::set_level(type_t type, level_t new_level) {
    level[(uint8_t) type] = new_level;
}
