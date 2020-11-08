//
// Created by noeel on 22-03-20.
//

#ifndef ARUNA_DSHOT_H
#define ARUNA_DSHOT_H
#define RMT_CHANNEL_FLAGS_ALWAYS_ON 1

#include <driver/rmt.h>
#include "aruna.h"

namespace aruna {
    namespace control {
        class Dshot : public Actuator {
        private:
            rmt_config_t driver_config;
            const axis_mask_t axis;
            const static size_t rmt_size = 17;
            rmt_item32_t dshot_frame[rmt_size];

//                tick tock
            uint16_t T0H_ticks: 15;
            uint16_t T1H_ticks: 15;
            uint16_t T0L_ticks: 15;
            uint16_t T1L_ticks: 15;

//                threading
            pthread_mutex_t dshot_frame_lock;
            pthread_t update_handler;

            /**
             * convert bits to rmt frame
             * @param bits dshot package (16bit with speed, telemetry and CRC)
             * @param frame_buffer array of 17 (to include 0 terminator)
             */
            void bits_to_dshotFrame(uint16_t bits, rmt_item32_t *frame_buffer);

            /**
             * add CRC to end of bit stream.
             * @param bits to add CRC on
             */
            void add_CRC(uint16_t &bits);

            /**
             * updates ESC with current dshot buffer, blocks CPU
             */
            void update_task();

            /**
             * static function to help pthead
             * @param _this Dshot object
             * @return 0
             */
            static void *__update_task(void *_this);

            /**
             * craft dshot package
             * @param speed keep between 48-2047
             * @param telemetry 0 or 1
             * @return dshot bit package with CRC
             */
            uint16_t create_dshotFrame(uint16_t speed, uint8_t telemetry);

            err_t _set(axis_mask_t axisMask, int16_t speed) override;

        public:
            /**
             * Dshot150
             * @param axis, axis mask
             * @param channel, rmt channel
             * @param gpio_port, gpio port to send Dshot over.
             */
            Dshot(axis_mask_t axis, rmt_channel_t channel, gpio_num_t gpio_port);

            ~Dshot();

        };
    }
}


#endif //ARUNA_DSHOT_H
