//
// Created by noeel on 15-04-20.
//

#ifndef ARUNA_REPORTER_H
#define ARUNA_REPORTER_H

#include "aruna/arunaTypes.h"
#include "aruna/sis/sisTypes.h"
#include "aruna/comm/commTypes.h"
namespace aruna {
    namespace sis {
        namespace reporter {
//            managers all the perfromers and report to watcher
            const static comm::port_t port = 1;
            /**
             * start reporter to report all SIS activiry to the watcher over comm channels
             * @return err if failed to register comm channel.
             */
            err_t start();

            /**
             * alert watcher with given status_t
             * @param status
             */
            void alert(status_t *status);

//            register(Performer performer);
        }
    }
}
#endif //ARUNA_REPORTER_H
