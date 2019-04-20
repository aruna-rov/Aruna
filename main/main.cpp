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
#include <drivers/control/L293D.h>
#include "drivers/control.h"

Com COM;
ComDriver *uart_driver;
ControlAcceleratorDriver* l293d_driver;

const static char* LOG_TAG = "MAIN";


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

    ESP_LOGI(LOG_TAG, "hello world!");
    register_drivers();
    start_COM();

    control_start();
//	control_set_X_velocity(100, CONTROL_DIRECTION_PLUS);

//    test application

//    xTaskCreate(com_test_task, "COM test task", 2048, NULL, 0, NULL);
    xTaskCreate(start_blinky_task, "blinky_app", 2048, NULL, 0, NULL);

}

void register_drivers() {
//    TODO if there are more drivers, then this needs his own .cpp file

//  COM
// TODO error check
    uart_driver = new UART;
    COM.register_candidate_driver(uart_driver);

//  Control
// TODO error check
    l293d_driver = new L293D;
    control_register_driver(l293d_driver);
}

void start_COM() {
    ESP_LOGD(LOG_TAG, "COM start: %d", COM.start());
    ESP_LOGD(LOG_TAG, "COM status: %d", COM.get_status());
    ESP_LOGD(LOG_TAG, "COM driver: %s", COM.getName());
    ESP_LOGD(LOG_TAG, "COM speed: %d", COM.get_speed());
}
