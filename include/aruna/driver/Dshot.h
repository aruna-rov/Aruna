//
// Created by noeel on 22-03-20.
//

#ifndef ARUNA_DSHOT_H
#define ARUNA_DSHOT_H

#include "aruna/movement/Actuator.h"
#include "pthread.h"

namespace aruna {
    namespace driver {
        class Dshot: public movement::Actuator {
        private:
            bool _continues_write = false;
            pthread_mutex_t write_mut;
            const uint32_t frequency;
            bool bidirectional;


            /**
             * add CRC to end of bit stream.
             * @param bits to add CRC on
             */
            void add_CRC(uint16_t &bits);


            /**
             * craft dshot package
             * @param speed keep between 48-2047
             * @param telemetry 0 or 1
             * @return dshot bit package with CRC
             */
            uint16_t create_dshotFrame(uint16_t speed, uint8_t telemetry);

            err_t _set(movement::axis_mask_t axisMask, int16_t speed) override;

            /**
             * Write set Dshot frame continuously on the bus
             * @param dshot_frame: dhsot frame to send, already encoded in bits
             * @return
             * 		- err_t::OK success
             * 		- err_t::FAIL on failure.
             */
            err_t write_frame_continuous(uint16_t dshot_frame);
            
            /**
			 * Write set Dshot frame continuously on the bus
			 * @param dshot_frame: Dhsot frame to send, already encoded in bits
			 * @return
			 * 		- err_t::OK success
			 * 		- err_t::FAIL on failure.
			 */
            virtual err_t _write_frame_continuous(uint16_t dshot_frame) = 0;

        protected:
            uint16_t T0H_ns;
            uint16_t T1H_ns;
            uint16_t T0L_ns;
            uint16_t T1L_ns;
        public:
            /**
             * Dshot parent object
             * @param speed_hz: speed of the protocol
             * @param bidirectional: set to true if the ESC is bidirectional.
             */
            Dshot(uint32_t speed_hz = 150000, bool bidirectional = false);

            /**
             * get frequency of the driver
             * @return frequency in hertz.
             */
            uint32_t get_frequency();

            /**
             * Set speed, use negative values to rotate in the other direction if bidirectional is enabled.
             * @param speed: speed
             * @return
             * 		- err_t::OK success
			 * 		- err_t::FAIL on failure.
             */
            err_t set_speed(int16_t speed);

            /**
             * Is this Dshot driver birectional (can spin both ways)
             * @return true if is birectional, false if not.
             */
            bool get_bidirectional();

            /**
             * Set bidirectional property of the driver.
             * @param is_bidirectional: true is bidirectional, false if single directional.
             */
            void set_bidirectional(bool is_bidirectional);

            /**
             * Call the function after the constructor to arm the ESC. set_speed() will not work without arming the ESC.
             */
            void arm_ESC();

            ~Dshot();


        };
    }
}


#endif //ARUNA_DSHOT_H
