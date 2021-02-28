//
// Created by noeel on 22-03-20.
//

#include "aruna/driver/Dshot.h"
#include <stdlib.h>
#include <unistd.h>

using namespace aruna;
using namespace aruna::driver;


Dshot::Dshot(uint32_t speed_hz, bool bidirectional): frequency(speed_hz), bidirectional(bidirectional) {

    uint32_t max_ns = (1.f / (float) speed_hz) * 1000000000;

    const static float T0H_percentage = 0.375;
    const static float T1H_percentage = 0.75;
    const static float T0L_percentage = 1 - T0H_percentage;
    const static float T1L_percentage = 1 - T1H_percentage;

    T0H_ns = max_ns * T0H_percentage;
    T1H_ns = max_ns * T1H_percentage;
    T0L_ns = max_ns * T0L_percentage;
    T1L_ns = max_ns * T1L_percentage;

    pthread_mutexattr_t swm_att;
    pthread_mutexattr_init(&swm_att);
    pthread_mutex_init(&write_mut, &swm_att);
    pthread_mutexattr_destroy(&swm_att);

}


aruna::err_t Dshot::_set(movement::axis_mask_t axisMask, int16_t speed) {
   return set_speed(speed);
}

err_t Dshot::set_speed(int16_t speed) {
	const static uint16_t MAX_VALUE = 2047;
	const static uint16_t MIN_VALUE = 48;
	const static uint16_t MID_VALUE = (MAX_VALUE + MIN_VALUE) / 2;
	const static uint8_t telemetry = 0;

	uint16_t max, min = 0;
	if (bidirectional) {
		max = speed < 0 ? MID_VALUE : MAX_VALUE;
		min = speed < 0 ? MIN_VALUE : MID_VALUE;
	} else {
		max = MAX_VALUE;
		min = MIN_VALUE;
	}
	uint16_t adjusted_speed = (uint16_t) convert_range(abs(speed), max, min);
	uint16_t bit_frame = create_dshotFrame(adjusted_speed, telemetry);


	return write_frame_continuous(bit_frame);
}



void Dshot::add_CRC(uint16_t &bits) {
    uint8_t csum = 0;
    uint16_t csum_data = bits >> 4;
    for (uint8_t i = 0; i < 3; i++) {
        csum ^= csum_data;
        csum_data >>= 4;
    }
    csum &= 0xf;
    bits |= csum;
}



uint16_t Dshot::create_dshotFrame(uint16_t speed, uint8_t telemetry) {
    uint16_t bit_frame = 0;
    bit_frame = speed << 5;
    bit_frame |= telemetry << 4;
    add_CRC(bit_frame);
    return bit_frame;
}

void Dshot::arm_ESC() {

//    arm ESC
    uint16_t start_value = 48;
    uint8_t step = 50;
    uint8_t amount_of_steps = 40;

//    up hill
    for (int i = 0; i < amount_of_steps; ++i) {
    	write_frame_continuous(create_dshotFrame((step * i) + start_value, 0));
        usleep(1);
    }
//    down hill
    for (int i = amount_of_steps - 1; i > -1; --i) {
    	write_frame_continuous(create_dshotFrame((step * i) + start_value, 0));
        usleep(1);
    }
//    stay low for a little while
	write_frame_continuous(create_dshotFrame(start_value, 0));
	usleep(150);

    set_speed(0);

}

err_t Dshot::write_frame_continuous(uint16_t dshot_frame) {
    err_t e;
    pthread_mutex_lock(&write_mut);
    e = _write_frame_continuous(dshot_frame);
    pthread_mutex_unlock(&write_mut);
    return e;
}

Dshot::~Dshot() {
    pthread_mutex_destroy(&write_mut);
}

uint32_t Dshot::get_frequency() {
	return frequency;
}

bool Dshot::get_bidirectional() {
	return bidirectional;
}

void Dshot::set_bidirectional(bool is_bidirectional) {
	bidirectional = is_bidirectional;
}
