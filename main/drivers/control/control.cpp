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
    const char* LOG_TAG = "CONTROL";
    control_status_t control_status = CONTROL_STOPPED;
    TaskHandle_t control_com_handler;
    TaskHandle_t control_damping;
    std::set<ControlAcceleratorDriver*> drivers;
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
    xTaskCreate(control_com_handler_task, "control_com_handler", 2048, NULL, 12, &control_com_handler);
    xTaskCreate(control_damping_task, "control_damping", 2048, NULL, 12, &control_damping);
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
    set_X_speed((int) cm_per_second);
    return CONTROL_OK;
}

control_err_t set_X_speed(int speed) {
//    if()
    for(ControlAcceleratorDriver* d: drivers) {
        if ((d->get_control_mode() & CONTROL_X) > 0) {
            d->set_X_speed(speed);
        }
    }
    return CONTROL_OK;
}
