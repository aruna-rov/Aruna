//
// Created by noeel on 15-04-20.
//

#ifndef ARUNA_WATCHER_H
#define ARUNA_WATCHER_H

#include <aruna/arunaTypes.h>
#include <aruna/comm/commTypes.h>
#include "aruna/sis/sisTypes.h"

namespace aruna {
    namespace sis {
        namespace watcher {
//            watches the comm for sis reports
            const static comm::port_t port = 2;

            /**
             * start SIS report watcher thread
             * @return comm error
             */
            err_t start();

            /**
             * Watch SIS reports task
             */
            void * watch(void *);

            /**
             * set level of alert on SIS level.
             */
            void set_level(type_t type, level_t new_level);
        }
    }
}

#endif //ARUNA_WATCHER_H
