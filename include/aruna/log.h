//
// Created by noeel on 17-7-19.
//

#ifndef ARUNA_LOG_H
#define ARUNA_LOG_H

#include "stdarg.h"
#include "stddef.h"
#include "stdint.h"
namespace aruna { namespace log {
    enum class level_t: int {
        NONE = -1,
        VERBOSE = 4,
        DEBUG = 3,
        INFO = 2,
        WARNING = 1,
        ERROR = 0,
    };
    typedef int (*vprintf_like_t)(const char *, va_list);
    static const level_t default_level = level_t::INFO;


    struct channel_t {
        /**
         * Channel name, used to identify
         */
        const char* name;

        /**
         * max log level of channel
         */
        level_t level = default_level;

        channel_t(const char* name, level_t level = default_level);

        /**
         * log verbose message
         * @param format, char array
         * @param ... insert extra variables
         * @return how many bytes have been written.
         */
        int verbose(const char* format, ...);
        /**
         * log debug message
         * @param format, char array
         * @param ... insert extra variables
         * @return how many bytes have been written.
         */
        int debug(const char* format, ...);
        /**
         * log info message
         * @param format, char array
         * @param ... insert extra variables
         * @return how many bytes have been written.
         */
        int info(const char* format, ...);
        /**
         * log warning message
         * @param format, char array
         * @param ... insert extra variables
         * @return how many bytes have been written.
         */
        int warning(const char* format, ...);
        /**
         * log error message
         * @param format, char array
         * @param ... insert extra variables
         * @return how many bytes have been written.
         */
        int error(const char* format, ...);
        /**
         * dump array of bin data
         * @param level log level
         * @param bin pointer to array
         * @param size size of array
         * @return bytes written
         */
        int dump(level_t level, uint8_t *bin, size_t size);

        bool operator<(const channel_t &b) const {
            return this->name < b.name;
        }

        bool operator==(const char* name) const {
            return this->name == name;
        }

        private:
            int print(level_t level, const char *format);
            int out(const char *format, ...);
            unsigned long epoch();

    };

    /**
     * Set level of log channel
     * @param channel_name, name of the channel
     * @param new_level level_t max log level for the channel
     * @return 0 if channel is unavailable, 1 if succeded
     */
    int set_level(const char *channel_name, level_t new_level);

    /**
     * Set a new print function to log to a diffrent location
     * @param func new print function
     * @return old print function
     */
    vprintf_like_t set_print_function(vprintf_like_t func);

    /**
     * set maximum print level for all channels
     * @param level new max print level
     * @return old max print level
     */
    level_t set_max_level(level_t level);

    /**
     * convert level_t object to char
     * @param level level_t
     * @param buffer minimal size of 5.
     */
    void level_t_to_char(level_t level, char *buffer);

}}

#endif //ARUNA_LOG_H
