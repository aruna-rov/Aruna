//
// Created by noeel on 15-04-20.
//

#include "aruna/sis/reporter.h"
#include "aruna/comm.h"
#include "aruna/sis/watcher.h"
#include "aruna/log.h"

using namespace aruna;

namespace {
    comm::channel_t *comm_channel;
    log::channel_t *log_channel;
}
aruna::err_t aruna::sis::reporter::start() {
    comm_channel = new comm::channel_t(port);
//    TODO error check
    log_channel = new log::channel_t("SIS report");
    return comm_channel->register_err;
}

void sis::reporter::alert(status_t *status) {
//    encode status
    auto* buffer = static_cast<uint8_t *>(malloc(status->get_encode_size()));
    status->encode(buffer);
    err_t send_err = comm_channel->send(watcher::port, buffer, status->get_encode_size(), true);
    if ((uint8_t) send_err)
        log_channel->error("failed to send SIS report to watcher: %s", err_to_char.at(send_err));
    free(buffer);
}
