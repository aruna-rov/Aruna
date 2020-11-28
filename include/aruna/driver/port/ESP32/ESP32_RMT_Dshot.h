//
// Created by noeel on 08-11-20.
//

#ifndef ARUNA_ESP32_RMT_DSHOT_H
#define ARUNA_ESP32_RMT_DSHOT_H
#define RMT_CHANNEL_FLAGS_ALWAYS_ON 1

#include "aruna/driver/Dshot.h"
#include <driver/rmt.h>


namespace aruna {
    namespace driver {
        class ESP32_RMT_Dshot: Dshot{
        private:
            //                tick tock
            uint16_t T0H_ticks: 15;
            uint16_t T1H_ticks: 15;
            uint16_t T0L_ticks: 15;
            uint16_t T1L_ticks: 15;

//                threading
            pthread_mutex_t dshot_frame_lock;
            pthread_t update_handler;

            rmt_config_t driver_config;
            uint16_t dshot_frame = 0;

            /**
            * convert bits to rmt frame
            * @param bits dshot package (16bit with speed, telemetry and CRC)
            * @param frame_buffer array of 17 (to include 0 terminator)
            */
            void bits_to_dshotFrame(uint16_t bits, rmt_item32_t *frame_buffer);

            /**
             * updates ESC with current dshot buffer, blocks CPU
             */
            void update_task();

            /**
             * static function to help pthead
             * @param _this Dshot object
             * @return 0
             */
            static void *_update_task(void *_this);

            err_t _write_frame_continuous(uint16_t dshot_frame) override;


            /**
             * Write a single Dshot frame to the bus
             * @param dshot_frame: frame to be written
             * @return always err_t::OK
             */
            err_t single_write(uint16_t dshot_frame);

        public:
            /**
             * Dshot150 using RMT hardware
             * @param channel, rmt channel
             * @param gpio_port, gpio port to send Dshot over.
             */
            ESP32_RMT_Dshot(rmt_channel_t channel, gpio_num_t gpio_port);

            ~ESP32_RMT_Dshot();


        };

    }
}

#endif //ARUNA_ESP32_RMT_DSHOT_H
