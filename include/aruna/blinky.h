//
// Created by noeel on 23-3-19.
//

#ifndef ARUNA_BLINKY_H
#define ARUNA_BLINKY_H

namespace aruna {
    namespace blinky {
        enum LED_STATUS {
            LED_ON,
            LED_OFF,
            LED_TOGGLE
        };

    // TODO documentation
        void start_blinky_task(void *arg);

    // TODO documentation
    void set_led(LED_STATUS ls);
    }
}
#endif //ARUNA_BLINKY_H
