//
// Created by noeel on 9-12-18.
//

#include "main.h"
#include <drivers/Com.h>
#include "esp_log.h"
extern "C" void app_main(void) {
    Com COM;
    ESP_LOGI("MAIN", "hello world!");
    COM.start();
    ESP_LOGD("COM", "COM status: %d", COM.get_status());
}