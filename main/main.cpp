//
// Created by noeel on 9-12-18.
//

#include <main.h>
#include "app/Com.h"
#include "drivers/com/UART.h"
#include "esp_log.h"
#include <freertos/task.h>
#include <soc/uart_struct.h>

Com COM;
ComDriver *uart_driver;

void testAppHandeler(com_transmitpackage_t d) {
    ESP_LOGI("MAIN", "howdy, partner!");
    ESP_LOGI("MAIN", "data: '%s'", d.data);
    ESP_LOGI("MAIN", "data_length: '%d'", d.data_lenght);
    ESP_LOGI("MAIN", "from: '%d'", d.from_port);
    ESP_LOGI("MAIN", "me: '%d'", d.to_port);
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

    com_channel_t testapp = {
            .port = 1,
            .priority = 2,
            .handeler = &testAppHandeler
    };
//    TODO register channel needs redesign, all endpoint data as paramter. handeler should be used.
    ESP_LOGI("testApp", "register: %d", COM.register_channel(&testapp));
    ESP_LOGI("testApp", "send: %d", COM.send(&testapp, 0xDE, (char*) "My man!", 7));
    ESP_LOGI("testApp", "send: %d", COM.send(&testapp, 0xAA, (char*) "precies 32 bytes aan data size!!", 32));
    ESP_LOGI("testApp", "send: %d", COM.send(&testapp, 0xAE, (char*) "een overflow van data! ohh nee, wat gebeurt er nu?", 50));




}