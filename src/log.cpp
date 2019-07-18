//
// Created by noeel on 17-7-19.
//

#include <cstdio>
#include <cstdint>
#include <stdarg.h>
#include <cstring>
#include "aruna/log.h"
#include "set"

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))

#include "time.h"

#elif defined(ESP_PLATFORM)
#include "esp_log.h"
#endif
namespace aruna {
    namespace log {
//    private
//    variables
        std::set<channel_t *> channels;
        level_t max_level = level_t::VERBOSE;
        vprintf_like_t print_function = vprintf;

//    functions
        int register_channel(channel_t *channel);

//    public

        //    functions
        int channel_t::verbose(const char *format, ...) {
            char buffer[256];
            int n = 0;
            va_list argptr;

            va_start(argptr, format);
            vsprintf(buffer, format, argptr);
            n = print(level_t::VERBOSE, buffer);
            va_end(argptr);

            return n;
        }

        int channel_t::debug(const char *format, ...) {
            char buffer[256];
            int n = 0;
            va_list argptr;

            va_start(argptr, format);
            vsprintf(buffer, format, argptr);
            n = print(level_t::DEBUG, buffer);
            va_end(argptr);

            return n;
        }

        int channel_t::info(const char *format, ...) {
            char buffer[256];
            int n = 0;
            va_list argptr;

            va_start(argptr, format);
            vsprintf(buffer, format, argptr);
            n = print(level_t::INFO, buffer);
            va_end(argptr);

            return n;
        }

        int channel_t::warning(const char *format, ...) {
            char buffer[256];
            int n = 0;
            va_list argptr;

            va_start(argptr, format);
            vsprintf(buffer, format, argptr);
            n = print(level_t::WARNING, buffer);
            va_end(argptr);

            return n;
        }

        int channel_t::error(const char *format, ...) {
            char buffer[256];
            int n = 0;
            va_list argptr;

            va_start(argptr, format);
            vsprintf(buffer, format, argptr);
            n = print(level_t::ERROR, buffer);
            va_end(argptr);

            return n;
        }

        int channel_t::print(level_t level, const char *format) {
            char level_txt[5];
            if (((int) level < (int) this->level) || ((int) level > (int) max_level) || level == level_t::NONE) {
                return 0;
            }
            level_t_to_char(level, level_txt);
            return out("%s (%u) %s, %s\n", level_txt, epoch(), name, format);
        }

        channel_t::channel_t(const char *name, level_t level) : name(name), level(level) {
            if (!register_channel((channel_t *) this))
                perror("LOG ERROR inserting channel");
        }

        unsigned long channel_t::epoch() {
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
            return ::time(NULL);
#elif defined(ESP_PLATFORM)
            return esp_log_timestamp();
#endif
        }

        int channel_t::out(const char *format, ...) {
            va_list list;
            int n;
            va_start(list, format);
            n = print_function(format, list);
            va_end(list);
            return n;
        }

        int channel_t::dump(level_t level, uint8_t *bin, size_t size) {
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
//        TODO posix variant
            return 0;
#elif defined(ESP_PLATFORM)
            esp_log_level_t esp_log_level;
            switch (level) {
                case level_t::VERBOSE:
                    esp_log_level = ESP_LOG_VERBOSE;
                    break;
                case level_t::DEBUG:
                    esp_log_level = ESP_LOG_DEBUG;
                    break;
                case level_t::INFO:
                    esp_log_level = ESP_LOG_INFO;
                    break;
                case level_t::WARNING:
                    esp_log_level = ESP_LOG_WARN;
                    break;
                case level_t::ERROR:
                    esp_log_level = ESP_LOG_ERROR;
                    break;
                default:
                case log::level_t::NONE:
                    esp_log_level = ESP_LOG_NONE;
                    break;
            }
            ESP_LOG_BUFFER_HEXDUMP(name, bin, size, esp_log_level);
            return size;
#endif
        }

        int register_channel(channel_t *channel) {
            if (channels.find(channel) != channels.end()) {
                return 0;
            }
            return channels.insert(channel).second;
        }

        int set_level(const char *channel_name, level_t new_level) {
            int success = false;
            for (auto candidate: channels) {
                if (candidate->name == channel_name) {
                    success = true;
                    candidate->level = new_level;
                }
            }
            return success;
        }

        vprintf_like_t set_print_function(vprintf_like_t func) {
            vprintf_like_t old_func = print_function;
            print_function = func;
            return old_func;
        }

        level_t set_max_level(level_t level) {
            level_t old_max_level = max_level;
            max_level = level;
            return old_max_level;
        }

        void level_t_to_char(level_t level, char *buffer) {
            switch (level) {
                case log::level_t::NONE:
                    strcpy(buffer, "NONE ");
                    break;
                case level_t::VERBOSE:
                    strcpy(buffer, "VERBO");
                    break;
                case level_t::DEBUG:
                    strcpy(buffer, "DEBUG");
                    break;
                case level_t::INFO:
                    strcpy(buffer, "INFO ");
                    break;
                case level_t::WARNING:
                    strcpy(buffer, "WARN ");
                    break;
                case level_t::ERROR:
                    strcpy(buffer, "ERROR");
                    break;
                default:
                    strcpy(buffer, "lvler");
                    break;
            }
        }
    }
}