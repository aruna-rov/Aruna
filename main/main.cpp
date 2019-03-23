//
// Created by noeel on 9-12-18.
//

#include <main.h>
#include "app/Com.h"
#include "drivers/com/UART.h"
#include "esp_log.h"
#include <freertos/task.h>
#include <soc/uart_struct.h>
#include <app/blinky.h>

Com COM;
ComDriver *uart_driver;



void com_test_task(void * arg) {
    com_transmitpackage_t d;
    QueueHandle_t handler;

    com_channel_t testapp = {
            .port = 1,
            .priority = 2,
            .handeler = &handler
    };
    //    TODO register channel needs redesign, all endpoint data as paramter. handeler should be used.
    ESP_LOGI("com_test_task", "register: %d", COM.register_channel(&testapp));
    ESP_LOGI("com_test_task", "send: %d", COM.send(&testapp, 0xDE, (char*) "My man!", 7));
    ESP_LOGI("com_test_task", "send: %d", COM.send(&testapp, 0xAA, (char*) "precies 32 bytes aan data size!!", 32));
    ESP_LOGI("com_test_task", "send: %d", COM.send(&testapp, 0xAE, (char*) "een overflow van data! ohh nee, wat gebeurt er nu?", 50));

    while (1) {
        if (xQueueReceive(handler,(void *) &d, (portTickType) portMAX_DELAY)) {
            ESP_LOGI("com_test_task", "howdy, partner!");
            ESP_LOGI("com_test_task", "data: '%s'", d.data);
            ESP_LOGI("com_test_task", "data_length: '%d'", d.data_lenght);
            ESP_LOGI("com_test_task", "from: '%d'", d.from_port);
            ESP_LOGI("com_test_task", "me: '%d'", d.to_port);
        }
    }
    vTaskDelete(NULL);
}


extern "C" void app_main(void) {
//    TODO driver moet geregistreerd worden in de driver zelf
//      Dit is niet zo elegant.
    uart_driver = new UART;
    COM.register_candidate_driver(uart_driver);
    ESP_LOGI("MAIN", "hello world!");
    ESP_LOGD("COM", "COM start: %d", COM.start());
    ESP_LOGD("COM", "COM status: %d", COM.get_status());
    ESP_LOGD("COM", "COM driver: %s", COM.getName());
    ESP_LOGD("COM", "COM speed: %d", COM.get_speed());

//    test application

    xTaskCreate(com_test_task, "COM test task", 2048, NULL, 0, NULL);
    xTaskCreate(start_blinky_task, "blinky_app", 2048, NULL, 0, NULL);


//    vTaskStartScheduler();



}