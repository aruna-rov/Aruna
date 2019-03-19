//
// Created by noeel on 9-12-18.
//

#include "main.h"
#include "drivers/Com.h"
#include "drivers/com/UART.h"
#include "esp_log.h"
#include <freertos/task.h>
#include <soc/uart_struct.h>

Com COM;
ComDriver* uart_driver;
//void testAppHandeler(com_data_t d){
//
//}

void testUART_task(void *param){
//    uart_write_bytes(UART_NUM_0, (const char*)"AAis is a test string.\n", 23);
    uart_driver->start();
    com_transmitpackage_t transmitpackage = {
            1,
            2,
            "abc"
    };
    uart_driver->transmit(transmitpackage);
    vTaskDelete(NULL);
}

extern "C" void app_main(void) {
//    TODO driver moet geregistreerd worden in de driver zelf
//      Dit is niet zo elegant.
    uart_driver = new UART;
//    COM.register_candidate_driver(uart_driver);
    ESP_LOGI("MAIN", "hello world!");
//    ESP_LOGD("COM", "COM start: %d", COM.start());
//    ESP_LOGD("COM", "COM status: %d", COM.get_status());
//    ESP_LOGD("COM", "COM driver: %s", COM.getName());

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

    xTaskCreate(testUART_task, "UART_TEST", 2048, NULL, 10, NULL);
//    while(1)
//    {
//        ESP_LOGD("MAIN", "loop");
//        vTaskDelay(1000);
//    }



}