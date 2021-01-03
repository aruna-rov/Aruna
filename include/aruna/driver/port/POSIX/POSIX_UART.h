//
// Created by noeel on 2-7-19.
//

#ifndef ARUNA_POSIX_UART
#define ARUNA_POSIX_UART

#include "aruna/arunaTypes.h"
#include <aruna/comm/Link.h>
#include <aruna/log.h>
#include <aruna/driver/UART.h>
#include <termios.h>

namespace aruna::driver {

    class POSIX_UART : public UART {
    public:
        POSIX_UART(char *port, uint32_t baudrate);

        ~POSIX_UART();

        /**
         * Set options to the file description of the UART
         * @param options: termios containing options. Remember to not directly set bits and concat with `get_options()`.
         * @return err_t::OK if written success full, HARDWARE_FAILURE if not.
         */
        err_t set_options(struct termios * options);

        /**
         * Get options of the file description UART device.
         * @param options: termios buffer struct.
         * @return err_t::OK if read success full, HARDWARE_FAILURE if not.
         */
        err_t get_options(struct termios * options);

        /**
         * Set input to Canonical.
         * Canonical input is line-oriented.
         * Input characters are put into a buffer which can be edited interactively by the user until a CR (carriage return) or LF (line feed) character is received.
         * @return err_t::OK if successfull, HARDWARE_FAILURE if not.
         */
        err_t canonical_input();

        /**
         * Set input to raw. c_lflag is disabled.
         * @return err_t::OK if successfull, HARDWARE_FAILURE if not.
         */
        err_t raw_input();

        /**
         * Process output before sending it (enabled c_oflag)
         * @return err_t::OK if successfull, HARDWARE_FAILURE if not.
         */
        err_t processed_output();

        /**
         * Disabled output processing (disabled c_oflag)
         * @return err_t::OK if successfull, HARDWARE_FAILURE if not.
         */
        err_t raw_output();

        /**
         * Map Newline to CR-NL for output. (enabled processed_output)
         * @return err_t::OK if successfull, HARDWARE_FAILURE if not.
         */
        err_t map_NL_to_CRNL();

    private:
        int file_description;
        const char *PORT;
        aruna::log::channel_t *log;

        size_t _write(uint8_t *data, size_t dataSize) override;

        size_t _read(uint8_t *buffer, size_t buffer_size) override;

        uint32_t _set_baudrate(uint32_t new_baudrate) override;

        err_t _set_flowcontrol(flowcontrol_t new_flowcontrol) override;

        err_t _set_parity(parity_t parity_bit) override;

        err_t _set_stop_bit(stop_bit_t stop_bit) override;

        err_t _set_word_length(word_length_t word_length) override;
    };

}
#endif //ARUNA_POSIX_UART
