//
// Created by noeel on 9-12-18.
//

#include "main.h"
#include <drivers/Com.h>
#include "drivers/com/UART.h"
#include "esp_log.h"
Com COM;

extern "C" void app_main(void) {
//    TODO driver moet geregistreerd worden in de driver zelf
//      Dit is niet zo elegant.
    ComDriver* uart_driver = new UART;
    COM.register_candidate_driver(uart_driver);
    ESP_LOGI("MAIN", "hello world!");
    uart_driver->start();
//    ESP_LOGD("COM", "COM start: %d", COM.start());
    ESP_LOGD("COM", "COM status: %d", COM.get_status());
//    ESP_LOGD("COM", "COM driver: %s", COM.getName());

    while(1)
    {
        ESP_LOGD("MAIN", "loop");
        vTaskDelay(1000);
    }
}