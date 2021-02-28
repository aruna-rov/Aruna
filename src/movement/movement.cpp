//
// Created by noeel on 27-3-19.
//


#include <aruna/movement.h>
#include <set>
#include <aruna/comm.h>
#include "aruna/movement/Actuator.h"
#include <pthread.h>
#include "aruna/log.h"

namespace aruna { namespace movement {

namespace {
//    variables
    log::channel_t* log;
	status_t movement_status = status_t::STOPPED;
	pthread_t movement_comm_handler;
	std::set<Actuator *> drivers;

	axis_t<int16_t> currentSpeed = {0, 0, 0, 0, 0, 0};
	axis_t<int16_t> currentVelocity = {0, 0, 0, 0, 0, 0};
	axis_t<int16_t> currentDegree = {0, 0, 0, 0, 0, 0};
	axis_t<damping_t> currentDamping;


}

status_t start() {
//    check movement status.
	if (movement_status == status_t::RUNNING)
		return movement_status;

	log = new log::channel_t("Movement");
//    check if we got drivers
	if (drivers.empty()) {
		log->warning("Start failed: No drivers found!");
		return movement_status;
	}

//    start all drivers
	for (Actuator *d: drivers) {
		err_t stat = d->startup_error;
		if (stat != err_t::OK)
//            TODO print driver name?
			log->warning("Driver failed to start: %s", err_to_char.at(stat));
	}

//    start threads.
	pthread_create(&movement_comm_handler, NULL, comm_handler_task, NULL);
    movement_status = status_t::RUNNING;
	return movement_status;
}

status_t get_status() {
	return movement_status;
}

void* comm_handler_task(void *arg) {
	enum comm_commands_t {
		NO_COMMAND = 0x00,

//		basic functionality
		SET_SPEED = 0x01,
		GET_SPEED = 0x02,
		SET_VELOCITY = 0x03,
		GET_VELOCITY = 0x04,
		SET_DEGREE = 0x05,
		GET_DEGREE = 0x06,
		GET_DIRECTION = 0x07,

//		closed loop configuration
		SET_DAMPING = 0x10,
		GET_DAMPING = 0x11,

//		debugging
		SET_RUNNING_STATE = 0x20,
		GET_RUNNING_STATE = 0x21,
		TEST_SENSORS = 0x22,
		CALIBRATE_SENSORS = 0x23,
		GET_SUPPORTED_AXIS = 0x24,
		SET_SENSOR_OFFSET = 0x25,
		GET_SENSOR_OFFSET = 0x26,
		ACTIVATE_MPU = 0x27,
		GET_MPU_STATUS = 0x28,

//		advanched automated control
		RETURN_HOME = 0x30,
		SET_HOME = 0x31,
		GOTO_CORDINATES = 0x32,
	};
//	initialize comms
	comm::transmitpackage_t request;
	uint8_t mask;
	uint8_t flags;
	int16_t data;
	uint8_t buffer[6];
	axis_mask_t active_axis;
	comm::channel_t movement_channel = {
			.port = 3,
	};
	int16_t (*get_value)(axis_mask_t mode);
	void (*set_value)(axis_mask_t mode, int16_t speed);
	comm_commands_t command;
	if((int) movement_channel.register_err) {
		log->error("failed to register comm channel: %s", err_to_char.at(movement_channel.register_err));
	}
	while (1) {
		if (movement_channel.receive(&request)) {
//			if (request.data_received_lenght < 2)
//				continue;
			flags = request.data_received[1];
			data = ((uint16_t) request.data_received[2] << 8) | request.data_received[3];
			get_value = nullptr;
			set_value = nullptr;
			command = NO_COMMAND;
			log->debug("comm command:%X, flag:%X, data:%d", request.data_received[0], request.data_received[1], data);
			switch (request.data_received[0]) {
//				basic functionality

				case GET_DEGREE:
					if ((flags & ((uint8_t) axis_mask_t::X | (uint8_t) axis_mask_t::Y | (uint8_t) axis_mask_t::Z)))
						break;
					get_value = get_degree;
					command = GET_DEGREE;
				case GET_VELOCITY:
					if (command == NO_COMMAND) {
						get_value = get_velocity;
						command = GET_VELOCITY;
					}
				case GET_SPEED:
					if (command == NO_COMMAND) {
						command = GET_SPEED;
						get_value = get_speed;
					}
					mask = 1;
					int16_t speed;
					active_axis = get_active_axis();
					for (int i = 0; i < (uint8_t) axis_mask_t::MAX; ++i) {
						mask = 0b1 << i;
						if (mask & flags & (uint8_t) active_axis) {
							buffer[0] = command;
							buffer[1] = mask;
							speed = get_value((axis_mask_t) mask);
							buffer[2] = (speed >> 8);
							buffer[3] = speed & 0xff;
//							log->debug("mask: %X flag: %X", mask, flags);
//							log->debug("GET_COMMAND: %X, value: %d",command,  speed);
//							TODO error check comm.send return value
							movement_channel.send(request.from_port, buffer, 4);
						}
					}
					break;
//				set target velocity yaw, roll, pitch, x, y, z
				case SET_DEGREE:
					if ((flags & ((uint8_t) axis_mask_t::X | (uint8_t) axis_mask_t::Y | (uint8_t) axis_mask_t::Z)))
						break;
					set_value = set_degree;
					command = SET_DEGREE;
				case SET_VELOCITY:
					if (command == NO_COMMAND) {
						set_value = set_velocity;
						command = SET_VELOCITY;
					}
				case SET_SPEED:
					if (command == NO_COMMAND) {
						set_value = set_speed;
						command = SET_SPEED;
					}
					log->verbose("command: %X, axis: 0X%02X, speed: %i", command, flags, data);
					set_value((axis_mask_t) flags, data);
					break;
//				control

//				get damping
				case GET_DAMPING:
					mask = 1;
					active_axis = get_active_axis();
					for (int j = 0; j < (uint8_t) axis_mask_t::MAX; ++j) {
						mask = 1 << j;
						if (mask & flags & (uint8_t) active_axis) {
							buffer[0] = GET_DAMPING;
							buffer[1] = mask;
							buffer[2] = (uint8_t) get_damping((axis_mask_t) mask);
							movement_channel.send(request.from_port, buffer, 3);
						}
					}
					break;
//				set damping
				case SET_DAMPING:
                    set_damping((axis_mask_t) flags, (damping_t) data);
					break;

//				debugging

//				get running state
				case GET_RUNNING_STATE:
					buffer[0] = GET_RUNNING_STATE;
					buffer[1] = (uint8_t) get_status();
					movement_channel.send(request.from_port, buffer, 2);
					break;
//				set running state
				case SET_RUNNING_STATE:
//					TODO return success?
					switch (data) {
						case (uint8_t) status_t::RUNNING :
                            start();
							break;
						case (uint8_t) status_t::STOPPED:
                            stop();
							break;
						default:
							break;
					}
					break;
//				test sensors
				case TEST_SENSORS:
					uint8_t t;
					t = test_sensor();
					buffer[0] = TEST_SENSORS;
					buffer[1] = t;
					movement_channel.send(request.from_port, buffer, 2);
					break;
//				calibrate sensors
				case CALIBRATE_SENSORS:
                    calibrate_sensors();
					break;
//				get supported modes
				case GET_SUPPORTED_AXIS:
					buffer[0] = GET_SUPPORTED_AXIS;
					buffer[1] = (uint8_t) get_active_axis();
					movement_channel.send(request.from_port, buffer, 2);
					break;
				case SET_SENSOR_OFFSET:
				case GET_SENSOR_OFFSET:
					break;
				case ACTIVATE_MPU:
//					TODO disable support also?
					break;
				case GET_MPU_STATUS:
					buffer[0] = GET_MPU_STATUS;
					movement_channel.send(request.from_port, buffer, 1);
					break;
//				advanced automated control

//				return home
				case RETURN_HOME:
//				set home
				case SET_HOME:
//				goto cordinates
				case GOTO_CORDINATES:
//				protocol error
				default:
					break;
			}
			movement_channel.receive_clear();
		}
	}
	pthread_exit(0);
}

status_t stop() {
	if (movement_status == status_t::STOPPED)
		return movement_status;

	//    stop all drivers
	for (Actuator *d: drivers) {
	    delete d;
	}

//    stop task
    pthread_cancel(movement_comm_handler);
    movement_status = status_t::STOPPED;
	return movement_status;
}

err_t register_driver(Actuator *driver) {
	if (drivers.find(driver) != drivers.end()) {
		return err_t::DRIVER_EXISTS;
	}
	if (!drivers.insert(driver).second)
		return err_t::BUFFER_OVERFLOW;
	return err_t::OK;
}

void calibrate_sensors() {
//	 TODO
}

uint8_t test_sensor() {
// TODO
	return 1;
}

void set_speed(axis_mask_t axisMask, int16_t speed) {
	uint8_t j = 1;
	err_t ret;
	axis_mask_t active_axis = get_active_axis();
	for (Actuator *d: drivers) {
		ret = d->set(axisMask, speed);
		if (ret != err_t::OK) {
			log->warning("setting speed of driver failed: %s", err_to_char.at(ret));
		}
	}
	for (uint i = 0; i < (uint8_t) axis_mask_t::MAX; i++) {
		j= 1 << i;
		if ((uint8_t) axisMask & j & (uint8_t) active_axis) {
			currentSpeed[j] = speed;
		}
	}
}

int16_t get_speed(axis_mask_t single_axis) {
	return currentSpeed[(uint8_t) single_axis];
}

int16_t get_velocity(axis_mask_t single_axis) {
	return currentVelocity[(uint8_t) single_axis];
}

int16_t get_degree(axis_mask_t single_axis) {
	if ((uint8_t) single_axis & ((uint8_t) axis_mask_t::X | (uint8_t) axis_mask_t::Y | (uint8_t) axis_mask_t::Z))
		return 0;
	return currentDegree[(uint8_t) single_axis];
}


void set_degree(axis_mask_t axisMask, int16_t degree) {
//	TODO
}

void set_velocity(axis_mask_t axisMask, int16_t mm_per_second) {
// TODO
}

axis_mask_t get_active_axis() {
	uint8_t modes = 0;
	for (Actuator *d: drivers) {
		modes |= (uint8_t) d->get_axis();
	}
	return (axis_mask_t) modes;
}

damping_t get_damping(axis_mask_t single_axis) {
	return currentDamping[(uint8_t) single_axis];
}

void set_damping(axis_mask_t axisMask, damping_t damp) {
	uint8_t j = 1;
	axis_mask_t active_axis = get_active_axis();
	for (uint8_t i = 0; i < (uint8_t) axis_mask_t::MAX; i++) {
		j=1<<i;
		if ((uint8_t) axisMask & j & (uint8_t) active_axis) {
			currentDamping[j] = damp;
		}
	}
}
}}
