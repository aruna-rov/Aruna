//
// Created by noeel on 27-3-19.
//


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/FreeRTOSConfig.h>
#include <control.h>
#include <set>
#include <esp_log.h>
#include <Com.h>
#include "drivers/control/ControlActuatorDriver.h"
#include "MPU.hpp"
#include "mpu/math.hpp"   // math helper for dealing with MPU data
#include "mpu/types.hpp"  // MPU data types and definitions
#include "I2Cbus.hpp"


namespace {
//    variables
	const char *LOG_TAG = "CONTROL";
	control_status_t control_status = CONTROL_STOPPED;
	TaskHandle_t control_com_handler;
	TaskHandle_t control_damping;
	std::set<ControlActuatorDriver *> drivers;
	const gpio_num_t I2C_SDA_PIN = GPIO_NUM_26;
	const gpio_num_t I2C_CLK_PIN = GPIO_NUM_25;
	constexpr uint I2C_CLK_SPEED = 400;

	constexpr gpio_num_t MPU_interrupt_pin = GPIO_NUM_15;  // GPIO_NUM
	constexpr uint16_t MPU_samplerate = 100;  // Hz
	constexpr mpud::accel_fs_t MPU_AccelFS = mpud::ACCEL_FS_16G;
	constexpr mpud::gyro_fs_t MPU_GyroFS = mpud::GYRO_FS_250DPS;
	constexpr mpud::dlpf_t MPU_DLPF = mpud::DLPF_188HZ;
	MPU_t MPU;         // create an object
	constexpr mpud::mpu_i2caddr_t MPU_12C_address = mpud::MPU_I2CADDRESS_AD0_LOW;
	constexpr mpud::int_config_t MPU_InterruptConfig{
			.level = mpud::INT_LVL_ACTIVE_HIGH,
			.drive = mpud::INT_DRV_PUSHPULL,
			.mode  = mpud::INT_MODE_PULSE50US,
			.clear = mpud::INT_CLEAR_STATUS_REG  //
	};
	mpud::float_axes_t gyro, accel;
	float roll{0}, pitch{0}, yaw{0};
	control_axis_t<uint16_t> currentSpeed = {0, 0, 0, 0, 0, 0};
	control_axis_t<uint16_t> currentVelocity = {0, 0, 0, 0, 0, 0};
	control_axis_t<uint16_t> currentDegree = {0, 0, 0, 0, 0, 0};
	control_axis_t<control_direction_t> currentDirection;
	control_axis_t<control_damping_t> currentDamping;
	bool MPU_active = false;


}

/**
 * MPU interrupt serivce routine.
 * @param taskHandle
 */
static IRAM_ATTR void mpuISR(TaskHandle_t taskHandle);

/**
 * Start the MPU. Bind output to `MPU_active`
 * @return return 0 if MPU fails. And 1 if its successfully turned on
 */
bool start_MPU();

control_status_t control_start() {
//    check control status.
	if (control_status == CONTROL_RUNNING)
		return control_status;

//    check if we got drivers
	if (drivers.empty()) {
		ESP_LOGW(LOG_TAG, "Start failed: No drivers found!");
		return control_status;
	}
	//	MPU6050 max i2c clock is 400khz
	i2c0.begin(I2C_SDA_PIN, I2C_CLK_PIN, I2C_CLK_SPEED);  // initialize the I2C bus


	MPU_active = start_MPU();
	if (!MPU_active)
		ESP_LOGI(LOG_TAG, "MPU functionality disabled.");
//    start all drivers
	for (ControlActuatorDriver *d: drivers) {
		control_err_t stat = d->start();
		if (stat != CONTROL_OK)
//            TODO print driver name?
			ESP_LOGW(LOG_TAG, "Driver failed to start:%d", stat);
	}

//    start threads.
	xTaskCreate(control_com_handler_task, "control_com_handler", 2048, NULL, 12, &control_com_handler);
	control_status = CONTROL_RUNNING;
	return control_status;
}

control_status_t control_get_status() {
	return control_status;
}

void control_com_handler_task(void *arg) {
	enum com_commands_t {
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
//	initialize coms
	com_transmitpackage_t request;
	uint8_t mask;
	uint8_t flags;
	uint16_t data;
	uint8_t buffer[6];
	control_axis_mask_t active_axis;
	QueueHandle_t control_com;
	com_channel_t control_channel = {
			.port = 3,
			.priority = 1,
			.handeler = &control_com,
	};
	uint16_t (*get_value)(control_axis_mask_t mode);
	void (*set_value)(control_axis_mask_t mode, uint16_t speed, control_direction_t direction);
	com_commands_t command;
	COM.register_channel(&control_channel);
	while (1) {
		if (xQueueReceive(control_com, &request, (portTickType) portMAX_DELAY)) {
//			if (request.data_received_lenght < 2)
//				continue;
			flags = request.data_received[1];
			data = ((uint16_t) request.data_received[2] << 8) | request.data_received[3];
			get_value = nullptr;
			set_value = nullptr;
			command = NO_COMMAND;
			ESP_LOGD(LOG_TAG, "com command:%X, flag:%X, data:%d", request.data_received[0], request.data_received[1], data);
			switch (request.data_received[0]) {
//				basic functionality

				case GET_DEGREE:
					if ((flags & (CONTROL_AXIS_MASK_X | CONTROL_AXIS_MASK_Y | CONTROL_AXIS_MASK_Z)) || !MPU_active)
						break;
					get_value = control_get_degree;
					command = GET_DEGREE;
				case GET_VELOCITY:
					if (!MPU_active)
						break;
					if (command == NO_COMMAND) {
						get_value = control_get_velocity;
						command = GET_VELOCITY;
					}
				case GET_SPEED:
					if (command == NO_COMMAND) {
						command = GET_SPEED;
						get_value = control_get_speed;
					}
					mask = 1;
					uint16_t speed;
					active_axis = control_get_active_axis();
					for (int i = 0; i < CONTROL_AXIS_MASK_MAX; ++i) {
						mask = 0b1 << i;
						if (mask & flags & active_axis) {
							buffer[0] = command;
							buffer[1] = mask;
							speed = get_value((control_axis_mask_t) mask);
							buffer[2] = (speed >> 8);
							buffer[3] = speed & 0xff;
//							ESP_LOGD(LOG_TAG, "mask: %X flag: %X", mask, flags);
//							ESP_LOGD(LOG_TAG, "GET_COMMAND: %X, value: %d",command,  speed);
//							TODO error check com.send return value
							COM.send(&control_channel, request.from_port, buffer, 4);
						}
					}
					break;
				case GET_DIRECTION:
					buffer[0] = GET_DIRECTION;
					mask = 1;
					active_axis = control_get_active_axis();
					for (int i = 0; i < CONTROL_AXIS_MASK_MAX; ++i) {
						mask = 1 << i;
						if (mask & flags & active_axis) {
							buffer[1] = mask;
							buffer[2] = control_get_direction((control_axis_mask_t) mask);
							COM.send(&control_channel, request.from_port, buffer, 3);
						}
					}
					break;
//				set target velocity yaw, roll, pitch, x, y, z
				case SET_DEGREE:
					if ((flags & (CONTROL_AXIS_MASK_X | CONTROL_AXIS_MASK_Y | CONTROL_AXIS_MASK_Z)) || !MPU_active)
						break;
					set_value = control_set_degree;
					command = SET_DEGREE;
				case SET_VELOCITY:
					if (!MPU_active)
						break;
					if (command == NO_COMMAND) {
						set_value = control_set_velocity;
						command = SET_VELOCITY;
					}
				case SET_SPEED:
					if (command == NO_COMMAND) {
						set_value = control_set_speed;
						command = SET_SPEED;
					}
					control_direction_t dir;
					dir = ((request.data_received[1] >> 6) & 0b1) ? CONTROL_DIRECTION_MIN : CONTROL_DIRECTION_PLUS;
					ESP_LOGV(LOG_TAG, "set: %X value: %d, dir: %d", command, data, dir);
					set_value((control_axis_mask_t) flags, data, dir);
					break;
//				control

//				get damping
				case GET_DAMPING:
					mask = 1;
					active_axis = control_get_active_axis();
					for (int j = 0; j < CONTROL_AXIS_MASK_MAX; ++j) {
						mask = 1 << j;
						if (mask & flags & active_axis) {
							buffer[0] = GET_DAMPING;
							buffer[1] = mask;
							buffer[2] = (uint8_t) control_get_damping((control_axis_mask_t) mask);
							COM.send(&control_channel, request.from_port, buffer, 3);
						}
					}
					break;
//				set damping
				case SET_DAMPING:
					control_set_damping((control_axis_mask_t) flags, (control_damping_t) data);
					break;

//				debugging

//				get running state
				case GET_RUNNING_STATE:
					buffer[0] = GET_RUNNING_STATE;
					buffer[1] = control_get_status();
					COM.send(&control_channel, request.from_port, buffer, 2);
					break;
//				set running state
				case SET_RUNNING_STATE:
//					TODO return success?
					switch (data) {
						case CONTROL_RUNNING :
							control_start();
							break;
						case CONTROL_STOPPED:
							control_stop();
							break;
						default:
							break;
					}
					break;
//				test sensors
				case TEST_SENSORS:
					uint8_t t;
					t = control_test_sensor();
					buffer[0] = TEST_SENSORS;
					buffer[1] = t;
					COM.send(&control_channel, request.from_port, buffer, 2);
					break;
//				calibrate sensors
				case CALIBRATE_SENSORS:
					MPU_active = start_MPU();
					if(MPU_active)
						control_calibrate_sensors();
					break;
//				get supported modes
				case GET_SUPPORTED_AXIS:
					buffer[0] = GET_SUPPORTED_AXIS;
					buffer[1] = control_get_active_axis();
					COM.send(&control_channel, request.from_port, buffer, 2);
					break;
				case SET_SENSOR_OFFSET:
				case GET_SENSOR_OFFSET:
					break;
				case ACTIVATE_MPU:
					MPU_active = start_MPU();
//					TODO disable support also?
					break;
				case GET_MPU_STATUS:
					buffer[0] = GET_MPU_STATUS;
					buffer[1] = MPU_active;
					COM.send(&control_channel, request.from_port, buffer, 2);
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
		}
	}
	vTaskDelete(NULL);
}

void control_damping_task(void *arg) {
	static constexpr double kRadToDeg = 57.2957795131;
	static constexpr float kDeltaTime = 1.f / MPU_samplerate;
//	static constexpr double g = 9.80665;
	constexpr uint16_t kFIFOPacketSize = 12;
	uint16_t fifocount;
	uint8_t buffer[kFIFOPacketSize];
	mpud::raw_axes_t rawAccel, rawGyro;
	float gyroRoll;
	float gyroPitch;
	float gyroYaw;
	float accelRoll;
	float accelPitch;
	while (true) {
		uint32_t notificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
//		TODO MPU unplug while running should be handled better.
		if (notificationValue > 1) {
			ESP_LOGW(LOG_TAG, "Task Notification higher than 1, value: %d", notificationValue);
			MPU.resetFIFO();
			continue;
		}
		// Check FIFO count
		fifocount = MPU.getFIFOCount();
		if (esp_err_t err = MPU.lastError()) {
			ESP_LOGE(LOG_TAG, "Error reading fifo count, %#X", err);
			MPU.resetFIFO();
			continue;
		}
		if (fifocount > kFIFOPacketSize * 2) {
			if (!(fifocount % kFIFOPacketSize)) {
				ESP_LOGE(LOG_TAG, "Sample Rate too high!, not keeping up the pace!, count: %d", fifocount);
			} else {
				ESP_LOGE(LOG_TAG, "FIFO Count misaligned! Expected: %d, Actual: %d", kFIFOPacketSize, fifocount);
			}
			MPU.resetFIFO();
			continue;
		}
		// Burst read data from FIFO

		if (esp_err_t err = MPU.readFIFO(kFIFOPacketSize, buffer)) {
			ESP_LOGE(LOG_TAG, "Error reading sensor data, %#X", err);
			MPU.resetFIFO();
			continue;
		}
		// Format
		rawAccel.x = buffer[0] << 8 | buffer[1];
		rawAccel.y = buffer[2] << 8 | buffer[3];
		rawAccel.z = buffer[4] << 8 | buffer[5];
		rawGyro.x = buffer[6] << 8 | buffer[7];
		rawGyro.y = buffer[8] << 8 | buffer[9];
		rawGyro.z = buffer[10] << 8 | buffer[11];
		// Calculate tilt angle
		// range: (roll[-180,180]  pitch[-90,90]  yaw[-180,180])

		gyroRoll = roll + mpud::math::gyroDegPerSec(rawGyro.x, MPU_GyroFS) * kDeltaTime;
		gyroPitch = pitch + mpud::math::gyroDegPerSec(rawGyro.y, MPU_GyroFS) * kDeltaTime;
		gyroYaw = yaw + mpud::math::gyroDegPerSec(rawGyro.z, MPU_GyroFS) * kDeltaTime;
		accelRoll = atan2(-rawAccel.x, rawAccel.z) * kRadToDeg;
		accelPitch = atan2(rawAccel.y, sqrt(rawAccel.x * rawAccel.x + rawAccel.z * rawAccel.z)) * kRadToDeg;
		// Fusion
		roll = gyroRoll * 0.95f + accelRoll * 0.05f;
		pitch = gyroPitch * 0.95f + accelPitch * 0.05f;
		yaw = gyroYaw;
		// correct yaw
		if (yaw > 180.f)
			yaw -= 360.f;
		else if (yaw < -180.f)
			yaw += 360.f;

		accel = mpud::accelGravity(rawAccel, MPU_AccelFS);  // raw data to gravity
//		accelG.x *= g;
//		accelG.y *= g;
//		accelG.z *= g;

//		printf("Pitch: %+6.1f \t Roll: %+6.1f \t Yaw: %+6.1f \n", pitch, roll, yaw);
//		printf("accel:\t\t %+.2f\t\t %+.2f\t\t %+.2f\n", accel.x, accel.y, accel.z);

	}
	vTaskDelete(NULL);
}

control_status_t control_stop() {
	if (control_status == CONTROL_STOPPED)
		return control_status;

	//    stop all drivers
	for (ControlActuatorDriver *d: drivers) {
		control_err_t stat = d->stop();
		if (stat != CONTROL_OK)
//            TODO print driver name?
			ESP_LOGW(LOG_TAG, "Driver failed to stop:%d", stat);
	}

//    stop task
	vTaskDelete(control_com_handler);
	vTaskDelete(control_damping);
	control_status = CONTROL_STOPPED;
	return control_status;
}

control_err_t control_register_driver(ControlActuatorDriver *driver) {
	if (drivers.find(driver) != drivers.end()) {
		return CONTROL_ERR_DRIVER_EXISTS;
	}
	if (!drivers.insert(driver).second)
		return CONTROL_ERR_OVERFLOW;
	return CONTROL_OK;
}

void control_calibrate_sensors() {
	mpud::raw_axes_t g, a;
	ESP_ERROR_CHECK(MPU.computeOffsets(&a, &g));
	ESP_ERROR_CHECK(MPU.setAccelOffset(a));
	ESP_ERROR_CHECK(MPU.setGyroOffset(g));
}

uint8_t control_test_sensor() {
	mpud::selftest_t retSelfTest;
	esp_err_t tc;
	tc = MPU.testConnection();  // test connection with the chip, return is a error code
	if (tc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "MPU connection failed: %s", esp_err_to_name(tc));
		return 0;
	}
	ESP_ERROR_CHECK(MPU.selfTest(&retSelfTest));
	if (retSelfTest & (mpud::SELF_TEST_GYRO_FAIL | mpud::SELF_TEST_ACCEL_FAIL)) {
		ESP_LOGE(LOG_TAG, "MPU selftest failed Gyro=%s Accel=%s",  //
				 (retSelfTest & mpud::SELF_TEST_GYRO_FAIL ? "FAIL" : "OK"),
				 (retSelfTest & mpud::SELF_TEST_ACCEL_FAIL ? "FAIL" : "OK"));
		return 0;
	}
	return 1;
}

static IRAM_ATTR void mpuISR(TaskHandle_t taskHandle) {
	BaseType_t HPTaskWoken = pdFALSE;
	vTaskNotifyGiveFromISR(taskHandle, &HPTaskWoken);
	if (HPTaskWoken == pdTRUE) portYIELD_FROM_ISR();
}

bool start_MPU() {

	if(MPU_active)
		return true;
	MPU.setBus(i2c0);  // set communication bus, for SPI -> pass 'hspi'
	MPU.setAddr(MPU_12C_address);  // set address or handle, for SPI -> pass 'mpu_spi_handle'
	if(MPU.testConnection() != ESP_OK) {
		ESP_LOGW(LOG_TAG, "MPU not connected.");
		return false;
	}
	ESP_ERROR_CHECK(MPU.initialize());  // this will initialize the chip and set default configurations

	if (!control_test_sensor()) {
		ESP_LOGE(LOG_TAG, "MPU test failed.");
		return false;
	}

	control_calibrate_sensors();

	ESP_ERROR_CHECK(MPU.setSampleRate(MPU_samplerate));  // in (Hz)
	ESP_ERROR_CHECK(MPU.setAccelFullScale(MPU_AccelFS));
	ESP_ERROR_CHECK(MPU.setGyroFullScale(MPU_GyroFS));
	ESP_ERROR_CHECK(MPU.setDigitalLowPassFilter(MPU_DLPF));  // smoother data
	ESP_ERROR_CHECK(MPU.setInterruptEnabled(mpud::INT_EN_RAWDATA_READY));  // enable INT pin


//	setup fifo
	ESP_ERROR_CHECK(MPU.setFIFOConfig(mpud::FIFO_CFG_ACCEL | mpud::FIFO_CFG_GYRO));
	ESP_ERROR_CHECK(MPU.setFIFOEnabled(true));

	// Setup Interrupt
	constexpr gpio_config_t kGPIOConfig{
			.pin_bit_mask = (uint64_t) 0x1 << MPU_interrupt_pin,
			.mode         = GPIO_MODE_INPUT,
			.pull_up_en   = GPIO_PULLUP_DISABLE,
			.pull_down_en = GPIO_PULLDOWN_ENABLE,
			.intr_type    = GPIO_INTR_POSEDGE  //rising edge
	};
	xTaskCreate(control_damping_task, "control_damping", 2048, NULL, 12, &control_damping);

	ESP_ERROR_CHECK(gpio_config(&kGPIOConfig));
	ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_IRAM));
	ESP_ERROR_CHECK(gpio_isr_handler_add(MPU_interrupt_pin, mpuISR, control_damping));
	ESP_ERROR_CHECK(MPU.setInterruptConfig(MPU_InterruptConfig));
	ESP_ERROR_CHECK(MPU.setInterruptEnabled(mpud::INT_EN_RAWDATA_READY));
	// Ready to start reading
	ESP_ERROR_CHECK(MPU.resetFIFO());  // start clean

	return true;
}

void control_set_speed(control_axis_mask_t axisMask, uint16_t speed, control_direction_t direction) {
	uint8_t j = 1;
	control_err_t ret;
	control_axis_mask_t active_axis = control_get_active_axis();
	for (ControlActuatorDriver *d: drivers) {
		ret = d->set(axisMask, speed, direction);
		if (ret != CONTROL_OK) {
			ESP_LOGW(LOG_TAG, "setting speed of driver failed: %X", ret);
		}
	}
	for (uint i = 0; i < CONTROL_AXIS_MASK_MAX; i++) {
		j= 1 << i;
		if (axisMask & j & active_axis) {
			currentDirection[j] = direction;
			currentSpeed[j] = speed;
		}
	}
}

uint16_t control_get_speed(control_axis_mask_t single_axis) {
	return currentSpeed[single_axis];
}

uint16_t control_get_velocity(control_axis_mask_t single_axis) {
	return currentVelocity[single_axis];
}

uint16_t control_get_degree(control_axis_mask_t single_axis) {
	if (single_axis & (CONTROL_AXIS_MASK_X | CONTROL_AXIS_MASK_Y | CONTROL_AXIS_MASK_Z))
		return 0;
	return currentDegree[single_axis];
}

control_direction_t control_get_direction(control_axis_mask_t single_axis) {
	return currentDirection[single_axis];
}

void control_set_degree(control_axis_mask_t axisMask, uint16_t degree, control_direction_t direction) {
//	TODO
}

void control_set_velocity(control_axis_mask_t axisMask, uint16_t mm_per_second, control_direction_t direction) {
// TODO
}

control_axis_mask_t control_get_active_axis() {
	uint8_t modes = 0;
	for (ControlActuatorDriver *d: drivers) {
		modes |= d->get_axis();
	}
	return (control_axis_mask_t) modes;
}

control_damping_t control_get_damping(control_axis_mask_t single_axis) {
	return currentDamping[single_axis];
}

void control_set_damping(control_axis_mask_t axisMask, control_damping_t damp) {
	uint8_t j = 1;
	control_axis_mask_t active_axis = control_get_active_axis();
	for (uint8_t i = 0; i < CONTROL_AXIS_MASK_MAX; i++) {
		j=1<<i;
		if (axisMask & j & active_axis) {
			currentDamping[j] = damp;
		}
	}
}
