//
// Created by noeel on 27-3-19.
//


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/FreeRTOSConfig.h>
#include <drivers/control.h>
#include <set>
#include "drivers/control/ControlAcceleratorDriver.h"

namespace {
    static control_status_t control_status;
    TaskHandle_t control_com_handler;
    TaskHandle_t control_damping;
    std::set<ControlAcceleratorDriver*> drivers;
}

control_err_t control_start() {
//    check control status.
    if (control_status == CONTROL_RUNNING)
        return CONTROL_ERR_ALREADY_STARTED;

//    check if we got drivers
    if (drivers.empty())
        return CONTROL_ERR_NO_DRIVER_FOUND;

//    start threads.
    xTaskCreate(control_com_handler_task, "control_com_handler", 2048, NULL, 12, &control_com_handler);
    xTaskCreate(control_damping_task, "control_damping", 2048, NULL, 12, &control_damping);
    return CONTROL_OK;
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

control_err_t control_stop() {
    if(control_status == CONTROL_STOPPED)
        return CONTROL_ERR_NOT_STARTED;

//    stop task
    vTaskDelete(control_com_handler);
    vTaskDelete(control_damping);
    return CONTROL_OK;
}


control_err_t control_register_driver(ControlAcceleratorDriver *driver) {
    if (drivers.find(driver) != drivers.end()) {
        return CONTROL_ERR_DRIVER_EXISTS;
    }
    if (!drivers.insert(driver).second)
        return CONTROL_ERR_OVERFLOW;
    return CONTROL_OK;
}