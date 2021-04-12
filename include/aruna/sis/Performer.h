//
// Created by noeel on 15-04-20.
//

#ifndef ARUNA_PERFORMER_H
#define ARUNA_PERFORMER_H

#include <cstdint>
#include "aruna/sis/sisTypes.h"
#include <pthread.h>
#include <aruna/log.h>

namespace aruna {
    namespace sis {
        class Performer {
        private:
            status_t status;
        protected:
//            update thread
            pthread_t thread;
            pthread_cond_t do_update_con;
            pthread_mutex_t do_update_mut;
//
            log::channel_t *log;
            char log_tag[10] = "SIS perf";
//            use set_update()
            bool do_update = false;

//            update thread handlers
            void update_handler();

            static void *_update_handler(void *_this);

//            set to true to cancel creation of update thread.
            bool interrupt_based = false;
        public:
            /**
             * update frequency
             */
            uint32_t update_ms = 500;

            /**
             * Performs check to see if the system is in a secure state
             */
            Performer();

            ~Performer();

//            TODO actions to bring system back to a secure state
//            virtual void action() = 0;
//            virtual void action_abort() = 0;

            /**
             * Perform check to see if system is secure and return updated status.
             * @return status_t pointer
             */
            virtual status_t *update_status() = 0;

            /**
             * Use to pauze and resume the update process.
             * @param do_update, bool
             */
            void set_update(bool do_update);

        };

    }
}

#endif //ARUNA_PERFORMER_H
