//
// Created by noeel on 2-7-19.
//

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include "aruna/arunaTypes.h"
#include "aruna/driver/port/POSIX/POSIX_UART.h"
#include <aruna/log.h>
#include <errno.h>

namespace aruna::driver {
// cheatcheet: https://www.cmrr.umn.edu/~strupp/serial.html#config

    size_t POSIX_UART::_write(uint8_t *package, size_t package_size) {
        return ::write(file_description, package, package_size);
    }

    size_t POSIX_UART::_read(uint8_t *buffer, size_t buffer_size) {
        return ::read(file_description, buffer, buffer_size);
    }

    POSIX_UART::POSIX_UART(char *port, uint32_t baudrate) : PORT(port) {
        this->log = new aruna::log::channel_t("POSIX_UART");

        file_description = open(PORT, O_RDWR | O_NOCTTY | O_NDELAY);

        if (file_description == -1) {
            /*
             * Could not open the port.
             */
            log->error("Unable to open port \"%s\": %s", PORT, strerror(errno));
        }
        fcntl(file_description, F_SETFL, 0);

        // Read the conf of the port

        struct termios options;
        if ((int)get_options(&options))
            log->error("failed to read config of %s", PORT);

        options.c_cflag |= (CLOCAL | CREAD);

        if ((int)set_options(&options))
            log->error("failed to write options");

        set_baudrate(baudrate);
        set_word_length(UART::word_length_t::EIGHT);
        set_parity(UART::parity_t::DISABLED);
        raw_input();
        raw_output();
        set_flowcontrol(UART::flowcontrol_t::DISABLED);

    }

    POSIX_UART::~POSIX_UART() {
        close(file_description);
    }

    uint32_t POSIX_UART::_set_baudrate(uint32_t new_baudrate) {
        switch (new_baudrate) {
            case 0000000:
                new_baudrate = B0;
                break;
            case 50:
                new_baudrate = B50;
                break;
            case 75:
                new_baudrate = B75;
                break;
            case 110:
                new_baudrate = B110;
                break;
            case 134:
                new_baudrate = B134;
                break;
            case 150:
                new_baudrate = B150;
                break;
            case 200:
                new_baudrate = B200;
                break;
            case 300:
                new_baudrate = B300;
                break;
            case 600:
                new_baudrate = B600;
                break;
            case 1200:
                new_baudrate = B1200;
                break;
            case 1800:
                new_baudrate = B1800;
                break;
            case 2400:
                new_baudrate = B2400;
                break;
            case 4800:
                new_baudrate = B4800;
                break;
            case 9600:
                new_baudrate = B9600;
                break;
            case 19200:
                new_baudrate = B19200;
                break;
            case 38400:
                new_baudrate = B38400;
                break;
            case 57600  :
                new_baudrate = B57600;
                break;
            case 115200 :
                new_baudrate = B115200;
                break;
            case 230400 :
                new_baudrate = B230400;
                break;
            case 460800 :
                new_baudrate = B460800;
                break;
            case 500000 :
                new_baudrate = B500000;
                break;
            case 576000 :
                new_baudrate = B576000;
                break;
            case 921600 :
                new_baudrate = B921600;
                break;
            case 1000000:
                new_baudrate = B1000000;
                break;
            case 1152000:
                new_baudrate = B1152000;
                break;
            case 1500000:
                new_baudrate = B1500000;
                break;
            case 2000000:
                new_baudrate = B2000000;
                break;
            case 2500000:
                new_baudrate = B2500000;
                break;
            case 3000000:
                new_baudrate = B3000000;
                break;
            case 3500000:
                new_baudrate = B3500000;
                break;
            case 4000000:
                new_baudrate = B4000000;
                break;
            default:
                new_baudrate = 0;
                break;
        }

        struct termios options;

        if ((int)get_options(&options))
            log->error("failed to read config of %s", PORT);

        cfsetispeed(&options, new_baudrate);
        cfsetospeed(&options, new_baudrate);

        if ((int)set_options(&options))
            log->error("failed to write baudrate %i", new_baudrate);

        return new_baudrate;
    }

    err_t POSIX_UART::_set_word_length(word_length_t word_length) {
        uint32_t character_size;
        switch (word_length) {
            case UART::word_length_t::FIVE:
                character_size = CS5;
                break;
            case UART::word_length_t::SIX:
                character_size = CS6;
                break;
            case UART::word_length_t::SEVEN:
                character_size = CS7;
                break;
            default:
            case UART::word_length_t::EIGHT:
                character_size = CS8;
                break;
        }

        struct termios options;

        err_t err = get_options(&options);
        if ((int) err)
            return err;

        options.c_cflag &= ~CSIZE; /* Mask the character size bits */
        options.c_cflag |= character_size;    /* Select  data bits */

        return set_options(&options);

    }

    err_t POSIX_UART::_set_parity(UART::parity_t parity_bit) {
        struct termios options;

        err_t err = get_options(&options);
        if ((int) err)
            return err;

        switch (parity_bit) {
            case UART::parity_t::EVEN:
                options.c_cflag |= PARENB;
                options.c_cflag &= ~PARODD;
                options.c_cflag &= ~CSTOPB;
                options.c_iflag |= (INPCK | ISTRIP);
                break;
            case UART::parity_t::ODD:
                options.c_cflag |= PARENB;
                options.c_cflag |= PARODD;
                options.c_cflag &= ~CSTOPB;
                options.c_iflag |= (INPCK | ISTRIP);
                break;
            default:
            case UART::parity_t::DISABLED:
                options.c_cflag &= ~PARENB;
                options.c_cflag &= ~CSTOPB;
                break;
        }

        return set_options(&options);

    }

    err_t POSIX_UART::_set_flowcontrol(UART::flowcontrol_t new_flowcontrol) {
        struct termios options;

        err_t err = get_options(&options);
        if ((int) err)
            return err;
        switch (new_flowcontrol) {
            case UART::flowcontrol_t::HARDWARE:
#ifdef CNEW_RTSCTS
                options.c_cflag |= CNEW_RTSCTS;    /* Also called CRTSCTS */
#else
                return err_t::NOT_SUPPORTED;
#endif
                options.c_iflag &= ~(IXON | IXOFF | IXANY);
                break;
            case UART::flowcontrol_t::SOFTWARE:
                options.c_iflag |= (IXON | IXOFF | IXANY);
                options.c_cc[VSTART] = XON;
                options.c_cc[VSTOP] = XOFF;
#ifdef CNEW_RTSCTS
                options.c_cflag &= ~CNEW_RTSCTS;
#endif
                break;
            case UART::flowcontrol_t::HARDWARE_SOFTWARE:
                options.c_iflag |= (IXON | IXOFF | IXANY);
                options.c_cc[VSTART] = XON;
                options.c_cc[VSTOP] = XOFF;
#ifdef CNEW_RTSCTS
                options.c_cflag |= CNEW_RTSCTS;    /* Also called CRTSCTS */
#else
                return err_t::NOT_SUPPORTED;
#endif
                break;
            default:
            case UART::flowcontrol_t::DISABLED:
#ifdef CNEW_RTSCTS
                options.c_cflag &= ~CNEW_RTSCTS;
#endif
                options.c_iflag &= ~(IXON | IXOFF | IXANY);
                break;

        }

        return set_options(&options);
    }

    err_t POSIX_UART::_set_stop_bit(UART::stop_bit_t stop_bit) {
        struct termios options;

        err_t err = get_options(&options);
        if ((int) err)
            return err;
        switch (stop_bit) {
            case UART::stop_bit_t::ONE:
                options.c_cflag &= ~CSTOPB;
                break;
            case UART::stop_bit_t::TWO:
                options.c_cflag |= CSTOPB;
                break;
            default:
            case UART::stop_bit_t::ONE_HALF:
                return err_t::NOT_SUPPORTED;
                break;
        }


        return set_options(&options);
    }

    err_t POSIX_UART::canonical_input() {
        struct termios options;

        err_t err = get_options(&options);
        if ((int) err)
            return err;

        options.c_lflag |= (ICANON | ECHO | ECHOE);

        return set_options(&options);

    }

    err_t POSIX_UART::processed_output() {
        struct termios options;

        err_t err = get_options(&options);
        if ((int) err)
            return err;

        options.c_oflag |= OPOST;

        return set_options(&options);

    }

    err_t POSIX_UART::raw_output() {
        struct termios options;

        err_t err = get_options(&options);
        if ((int) err)
            return err;

        options.c_oflag &= ~OPOST;

        return set_options(&options);

    }

    err_t POSIX_UART::raw_input() {
        struct termios options;

        err_t err = get_options(&options);
        if ((int) err)
            return err;

        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        /*
         * Set the new options for the port...
         */
        return set_options(&options);

    }

    err_t POSIX_UART::map_NL_to_CRNL() {
        struct termios options;

        err_t err = get_options(&options);
        if ((int) err)
            return err;

        options.c_oflag |= OPOST;
        options.c_oflag |= ONLCR;

        return set_options(&options);
    }

    err_t POSIX_UART::set_options(struct termios *options) {
        /*
         * Set the new options for the port...
         */
        if (tcsetattr(file_description, TCSADRAIN, options) == -1) {
            log->error("Error with tcsetattr = %s", strerror(errno));
            return err_t::HARDWARE_FAILURE;
        }
        return err_t::OK;
    }

    err_t POSIX_UART::get_options(struct termios *options) {
        /*
         * Get the current options for the port...
         */
        if (tcgetattr(file_description, options) == -1) {
            log->error("Error with tcgetattr = %s", strerror(errno));
            return err_t::HARDWARE_FAILURE;
        }
        return err_t::OK;
    }

//    TODO set_options with lamba as paremeter

}
