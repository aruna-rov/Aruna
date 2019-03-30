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

namespace {
//    variables
    const char* LOG_TAG = "CONTROL";
    control_status_t control_status = CONTROL_STOPPED;
    TaskHandle_t control_com_handler;
    TaskHandle_t control_damping;
    std::set<ControlAcceleratorDriver*> drivers;

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
    control_err_t set_X_speed(uint32_t speed, control_direction_t direction) {
//    if()
        for(ControlAcceleratorDriver* d: drivers) {
            if ((d->get_control_mode() & CONTROL_X) > 0) {
                d->set_X_speed(speed, direction);
            }
        }
        return CONTROL_OK;
    }

}

control_status_t control_start() {
//    check control status.
    if (control_status == CONTROL_RUNNING)
        return control_status;

//    check if we got drivers
    if (drivers.empty()) {
        ESP_LOGW(LOG_TAG, "Start failed: No drivers found!");
        return control_status;
    }

//    start all drivers
    for (ControlAcceleratorDriver* d: drivers) {
        control_err_t stat = d->start();
        if(stat != CONTROL_OK)
//            TODO print driver name?
            ESP_LOGW(LOG_TAG, "Driver failed to start:%d", stat);
    }

//    start threads.
//    xTaskCreate(control_com_handler_task, "control_com_handler", 2048, NULL, 12, &control_com_handler);
//    xTaskCreate(control_damping_task, "control_damping", 2048, NULL, 12, &control_damping);
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
    while (1) {

    }
    vTaskDelete(NULL);
}

control_status_t control_stop() {
    if(control_status == CONTROL_STOPPED)
        return control_status;

    //    stop all drivers
    for (ControlAcceleratorDriver* d: drivers) {
        control_err_t stat = d->stop();
        if(stat != CONTROL_OK)
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
    uint32_t MAX = 0xffffffff;

    set_X_speed(MAX/50, CONTROL_FORWARD);
    return CONTROL_OK;
}

