//
// Created by noeel on 9-12-18.
//

#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <soc/uart_struct.h>
#include "aruna.h"

using namespace aruna;
namespace aruna {
    aruna::Com COM;
}
aruna::drivers::com::ComDriver *uart_driver;
aruna::drivers::control::ControlActuatorDriver* l293d_driver;
aruna::drivers::com::ComDriver *rs485_driver;

const static char* LOG_TAG = "MAIN";

extern "C" void app_main(void);
void testUART_task(void *param);
void start_COM();
void register_drivers();

void com_test_task(void * arg) {
    aruna::Com::transmitpackage_t d;
    QueueHandle_t handler;

    aruna::Com::channel_t testapp = {
            .port = 1,
            .priority = 1,
            .handeler = &handler
    };
    //    TODO register channel needs redesign, all endpoint data as paramter. handeler should be used.
    ESP_LOGI("com_test_task", "register: %d", (uint8_t) COM.register_channel(&testapp));
    ESP_LOGI("com_test_task", "send: %X", (uint8_t) COM.send(&testapp, 0xAA, (uint8_t *) "precies 32 bytes aan data size!\n", 32,
												   0));
    ESP_LOGI("com_test_task", "send: %X", (uint8_t) COM.send(&testapp, 0xAE,
												   (uint8_t *) "een overflow van data! ohh nee, wat gebeurt er nu?\n",
												   51, true));
	ESP_LOGI("com_test_task", "send: %X", (uint8_t) COM.send(&testapp, 0xDE, (uint8_t *) "My man!\n", 8, true));

    while (1) {
        if (xQueueReceive(handler,(void *) &d, (portTickType) portMAX_DELAY)) {
            ESP_LOGI("com_test_task", "howdy, partner!");
            ESP_LOGI("com_test_task", "data: '%s'", d.data_received);
            ESP_LOGI("com_test_task", "data_length: '%d'", d.data_lenght);
            ESP_LOGI("com_test_task", "from: '%d'", d.from_port);
            ESP_LOGI("com_test_task", "me: '%d'", d.to_port);
        }
    }
    vTaskDelete(NULL);
}


extern "C" void app_main(void) {

	esp_log_level_set("COM", ESP_LOG_VERBOSE);
	esp_log_level_set("UART", ESP_LOG_VERBOSE);
	ESP_LOGI(LOG_TAG, "hello world!");
    register_drivers();
    start_COM();

    aruna::control::start();
//	aruna::control::set_X_velocity(100, direction_t::PLUS);

//    test application

//		TODO task overflow?!
    xTaskCreate(com_test_task, "COM test task", 2048, NULL, 0, NULL);
    xTaskCreate(aruna::blinky::start_blinky_task, "blinky_app", 2048, NULL, 0, NULL);

}

void register_drivers() {
//    TODO if there are more drivers, then this needs his own .cpp file

//  COM
// TODO error check
    uart_driver = new aruna::drivers::com::UART;
    COM.register_candidate_driver(uart_driver);

	uart_config_t rs485_config = {
			.baud_rate = 5000000,
			.data_bits = UART_DATA_8_BITS,
			.parity = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
			.rx_flow_ctrl_thresh = 122,
			.use_ref_tick = false
	};
	rs485_driver = new aruna::drivers::com::UART((char*)"RS485",
							UART_NUM_1,
							23,
							22,
							18,
							UART_PIN_NO_CHANGE,
							rs485_config,
							UART_MODE_RS485_HALF_DUPLEX,
							256,
							512);
//	COM.register_candidate_driver(rs485_driver);


//  Control
// TODO error check
    l293d_driver = new aruna::drivers::control::L293D;
    aruna::control::register_driver(l293d_driver);
}

void start_COM() {
    ESP_LOGD(LOG_TAG, "COM start: %d", (uint8_t) COM.start());
    ESP_LOGD(LOG_TAG, "COM status: %d", (uint8_t) COM.get_status());
    ESP_LOGD(LOG_TAG, "COM driver: %s", (char*) COM.getName());
    ESP_LOGD(LOG_TAG, "COM speed: %d", (uint8_t) COM.get_speed());
}