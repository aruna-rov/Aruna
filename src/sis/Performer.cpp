//
// Created by noeel on 15-04-20.
//

#include "aruna/sis/reporter.h"
#include "aruna/sis/Performer.h"
#if defined(ESP_PLATFORM)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#else
#include <unistd.h>
#endif

using namespace aruna::sis;

aruna::sis::Performer::Performer(): status(type_t::DEFAULT) {
    log = new log::channel_t(log_tag);

    if (not interrupt_based) {
//    create task
        pthread_cond_init(&do_update_con, NULL);
        pthread_mutex_init(&do_update_mut, NULL);
        pthread_create(thread, NULL, _update_handler, this);
    }
// set standard values for status_t
    status.level = level_t::NOTIFY;
    status.type = type_t::DEFAULT;
    strcpy(status.description, "System is good");
}

void *aruna::sis::Performer::_update_handler(void *_this) {
    static_cast<Performer *>(_this)->update_handler();
    return nullptr;
}

void aruna::sis::Performer::update_handler() {
    status_t* status;
    while (1) {
        while (do_update) {
            status = update_status();
            sis::reporter::alert(status);
#if defined(ESP_PLATFORM)
            vTaskDelay(update_ms / portTICK_PERIOD_MS);
#else
//            usleep blocks CPU in ESP-IDF 3.2.2
//          TODO update to newer version of EPS-IDF
            usleep(update_ms);
#endif
        }
        pthread_mutex_lock(&do_update_mut);
        pthread_cond_wait(&do_update_con, &do_update_mut);
        pthread_mutex_unlock(&do_update_mut);
    }
}

void aruna::sis::Performer::set_update(bool do_update) {
    pthread_mutex_lock(&do_update_mut);
    this->do_update = do_update;
    pthread_mutex_unlock(&do_update_mut);
    pthread_cond_signal(&do_update_con);
}

Performer::~Performer() {
//    TODO if interrupt_based in changed between constructor and destructor,
//     it will try to delete things that don't exists or don't delete the thread.
    if (not interrupt_based) {
        pthread_cond_destroy(&do_update_con);
        pthread_mutex_destroy(&do_update_mut);
        pthread_exit(thread);
    }
}
