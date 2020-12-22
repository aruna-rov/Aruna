//
// Created by noeel on 16-08-19.
//

#ifndef ARUNA_ARUNATYPES_H
#define ARUNA_ARUNATYPES_H

// TODO make optional to only include these if FreeRTOS+POSIX is loaded.
// might be even better to just include `pthread.h` but that results in issues.
//extern "C" {
//    #include "FreeRTOS_POSIX_portable.h"
//    #include <FreeRTOS_POSIX/pthread.h>
//};

#include <cstdint>
#include "map"

namespace aruna {
	// state the status of the executed function, > 0 means no success
	enum class err_t : uint8_t {
		OK = 0,
		UNDEFINED = 1,
		FAIL = 100,

		// running status
        NOT_STOPPED = 101,
		NOT_STARTED = 102,
		NOT_PAUSED = 103,
		ALREADY_STARTED = 104, // TODO redundant because NOT_STOPPED is basicly the same thing

		//    hardware
        HARDWARE_FAILURE = 110,
		NO_CONNECTION = 112,
		NO_RESPONSE = 113,
		PROTOCOL_ERROR = 114,
		NO_HARDWARE_FOUND = 115,
		NOT_SUPPORTED = 116,

		//    config
        BUFFER_OVERFLOW = 120,
		INVALID_PARAMETERS = 121,
		TASK_FAILED = 122,
		BUFFER_UNDERFLOW = 123,

		//    channel/driver registation
        NO_CHANNEL = 130,
		CHANNEL_EXISTS = 131,
		NO_DRIVER = 132,
		DRIVER_EXISTS = 133,
		NO_ACTIVE_MODULES = 134,
		MODE_NOT_ACTIVE = 135,

	};
	const std::map<err_t, char *> err_to_char = {
			{err_t::OK,                 (char *) "OK"},
            {err_t::UNDEFINED,          (char *) "UNDEFINED"},
			{err_t::FAIL,               (char *) "FAIL"},

			// running status
			{err_t::NOT_STOPPED,        (char *) "NOT_STOPPED"},
			{err_t::NOT_STARTED,        (char *) "NOT_STARTED"},
			{err_t::NOT_PAUSED,         (char *) "NOT_PAUSED"},
			{err_t::ALREADY_STARTED,    (char *) "ALREADY_STARTED"},

			//    hardware
			{err_t::HARDWARE_FAILURE,   (char *) "HARDWARE_FAILURE"},
			{err_t::NO_CONNECTION,      (char *) "NO_CONNECTION"},
			{err_t::NO_RESPONSE,        (char *) "NO_RESPONSE"},
			{err_t::PROTOCOL_ERROR,     (char *) "PROTOCOL_ERROR"},
			{err_t::NO_HARDWARE_FOUND,  (char *) "NO_HARDWARE_FOUND"},
			{err_t::NOT_SUPPORTED,      (char *) "NOT_SUPPORTED"},

			//    config
			{err_t::BUFFER_OVERFLOW,    (char *) "BUFFER_OVERFLOW"},
			{err_t::INVALID_PARAMETERS, (char *) "INVALID_PARAMETERS"},
			{err_t::TASK_FAILED,        (char *) "TASK_FAILED"},
			{err_t::BUFFER_UNDERFLOW,   (char *) "BUFFER_UNDERFLOW"},

			//    channel/driver registation
			{err_t::NO_CHANNEL,         (char *) "NO_CHANNEL"},
			{err_t::CHANNEL_EXISTS,     (char *) "CHANNEL_EXISTS"},
			{err_t::NO_DRIVER,          (char *) "NO_DRIVER"},
			{err_t::DRIVER_EXISTS,      (char *) "DRIVER_EXISTS"},
			{err_t::NO_ACTIVE_MODULES,  (char *) "NO_ACTIVE_MODULES"},
			{err_t::MODE_NOT_ACTIVE,    (char *) "MODE_NOT_ACTIVE"},
	};

}

#endif //ARUNA_ARUNATYPES_H
