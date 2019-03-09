//
// Created by noeel on 9-12-18.
//

#include "main.h"
#include <drivers/Com.h>
#include "drivers/com/UART.h"
#include "esp_log.h"
Com COM;



void testAppHandeler(com_data_t d){

}

extern "C" void app_main(void) {
//    TODO driver moet geregistreerd worden in de driver zelf
//      Dit is niet zo elegant.
    ComDriver* uart_driver = new UART;
    COM.register_candidate_driver(uart_driver);
    ESP_LOGI("MAIN", "hello world!");
    uart_driver->start();
//    ESP_LOGD("COM", "COM start: %d", COM.start());
//    ESP_LOGD("COM", "COM status: %d", COM.get_status());
//    ESP_LOGD("COM", "COM driver: %s", COM.getName());
    com_transmitpackage_t transmitpackage = {
            .from_port = 1,
            .to_port = 2,
            .data = "123"
    };
    uart_driver->transmit(transmitpackage);

//    test application

//    com_endpoint_t testapp = {
//            .port = 1,
//            .priority = 2,
//            .handeler = &testAppHandeler
//    };
//    ESP_LOGD("dinges", "so far so good");
//    ESP_LOGI("testApp", "register: %d", COM.register_channel(&testapp));
//    com_data_t data = "hello world!";
//    com_datapackage_t testAppData = {
//            .com_endpoint = &testapp,
//            .to_port = 2,
//            .data = &data
//    };
//    ESP_LOGI("testApp", "send: %d", COM.send(testAppData));

    while(1)
    {
        ESP_LOGD("MAIN", "loop");
        vTaskDelay(1000);
    }
}