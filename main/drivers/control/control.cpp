//
// Created by noeel on 27-3-19.
//


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/FreeRTOSConfig.h>
#include <drivers/control.h>
#include <set>
#include <esp_log.h>
#include "drivers/control/ControlAcceleratorDriver.h"
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
	std::set<ControlAcceleratorDriver *> drivers;
	const gpio_num_t I2C_SDA_PIN = GPIO_NUM_26;
	const gpio_num_t I2C_CLK_PIN = GPIO_NUM_25;
	constexpr uint I2C_CLK_SPEED = 400;

	constexpr gpio_num_t MPU_interrupt_pin = GPIO_NUM_15;  // GPIO_NUM
	constexpr uint16_t MPU_samplerate = 100;  // Hz
	constexpr mpud::accel_fs_t MPU_AccelFS = mpud::ACCEL_FS_2G;
	constexpr mpud::gyro_fs_t MPU_GyroFS = mpud::GYRO_FS_500DPS;
	constexpr mpud::dlpf_t MPU_DLPF = mpud::DLPF_98HZ;
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


//    functions

	/**
	 * Set the speed of the engine/moter etc. directly
	 * @param speed, how fast the motor must spin (min-max must be defined by the motor hardware interface)
	 * @return control_err_t
	 *  * `CONTROL_OK` if is was a success.
	 *  * `CONTROL_ERR_NOT_STARTED` control not started yet. Use `control_start()`.
	 *  * `CONTROL_ERR_MODE_NOT_ACTIVE` mode is not active! Check `control_get_active_modes()`.
	 *  * `CONTROL_ERR_HARDWARE_FAILURE` the hardware failed you :(.
	 *  * `CONTROL_ERR_OVERFLOW` speed overflow.
	 *  * `CONTROL_ERR_UNDERFLOW` speed underflow.
	 */
	control_err_t set_X_speed(float speed, control_direction_t direction) {
//    if()
		for (ControlAcceleratorDriver *d: drivers) {
			if ((d->get_control_mode() & CONTROL_X) > 0) {
				d->set_X_speed(speed, direction);
			}
		}
		return CONTROL_OK;
	}

	control_err_t set_X_speed(float speed) {
		control_direction_t dir = speed >= 0 ? CONTROL_FORWARD : CONTROL_BACKWARD;
		speed = speed < 0 ? -speed : speed;
		return set_X_speed(speed, dir);
	};

}

static IRAM_ATTR void mpuISR(TaskHandle_t taskHandle);

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
	MPU.setBus(i2c0);  // set communication bus, for SPI -> pass 'hspi'
	MPU.setAddr(MPU_12C_address);  // set address or handle, for SPI -> pass 'mpu_spi_handle'
	MPU.initialize();  // this will initialize the chip and set default configurations
//	if (!control_test_sensor()) {
//		return CONTROL_STOPPED;
//	}

	control_calibrate_sensors();

	MPU.setSampleRate(MPU_samplerate);  // in (Hz)
	MPU.setAccelFullScale(MPU_AccelFS);
	MPU.setGyroFullScale(MPU_GyroFS);
	MPU.setDigitalLowPassFilter(MPU_DLPF);  // smoother data
	MPU.setInterruptEnabled(mpud::INT_EN_RAWDATA_READY);  // enable INT pin


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

	gpio_config(&kGPIOConfig);
	gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
	gpio_isr_handler_add(MPU_interrupt_pin, mpuISR, control_damping);
	ESP_ERROR_CHECK(MPU.setInterruptConfig(MPU_InterruptConfig));
	ESP_ERROR_CHECK(MPU.setInterruptEnabled(mpud::INT_EN_RAWDATA_READY));
	xTaskGetCurrentTaskHandle();
	// Ready to start reading
	ESP_ERROR_CHECK(MPU.resetFIFO());  // start clean

//    start all drivers
	for (ControlAcceleratorDriver *d: drivers) {
		control_err_t stat = d->start();
		if (stat != CONTROL_OK)
//            TODO print driver name?
			ESP_LOGW(LOG_TAG, "Driver failed to start:%d", stat);
	}

//    start threads.
//    xTaskCreate(control_com_handler_task, "control_com_handler", 2048, NULL, 12, &control_com_handler);
	control_status = CONTROL_RUNNING;
	return control_status;
}

control_status_t control_get_status() {
	return control_status;
}

void control_com_handler_task(void *arg) {
	while (1) {

	}
	vTaskDelete(NULL);
}

void control_damping_task(void *arg) {
	static constexpr double kRadToDeg = 57.2957795131;
	static constexpr float kDeltaTime = 1.f / MPU_samplerate;
	static constexpr double g = 9.80665;
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

		printf("Pitch: %+6.1f \t Roll: %+6.1f \t Yaw: %+6.1f \n", pitch, roll, yaw);
		printf("accel:\t\t %+.2f\t\t %+.2f\t\t %+.2f\n", accel.x, accel.y, accel.z);

	}
	vTaskDelete(NULL);
}

control_status_t control_stop() {
	if (control_status == CONTROL_STOPPED)
		return control_status;

	//    stop all drivers
	for (ControlAcceleratorDriver *d: drivers) {
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

control_err_t control_register_driver(ControlAcceleratorDriver *driver) {
	if (drivers.find(driver) != drivers.end()) {
		return CONTROL_ERR_DRIVER_EXISTS;
	}
	if (!drivers.insert(driver).second)
		return CONTROL_ERR_OVERFLOW;
	return CONTROL_OK;
}


// X

float control_get_X_velocity() {
	return 0;
}

control_err_t control_set_X_velocity(float cm_per_second) {
	return set_X_speed(cm_per_second);
}

void control_calibrate_sensors() {
	mpud::raw_axes_t g, a;
	MPU.computeOffsets(&a, &g);
	MPU.setAccelOffset(a);
	MPU.setGyroOffset(g);
}

uint8_t control_test_sensor() {
	mpud::selftest_t retSelfTest;
	esp_err_t tc;
	tc = MPU.testConnection();  // test connection with the chip, return is a error code
	if (tc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "MPU connection failed:%s", esp_err_to_name(tc));
		return 0;
	}
	MPU.selfTest(&retSelfTest);
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