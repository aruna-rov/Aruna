//
// Created by noeel on 9-12-18.
//

#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <soc/uart_struct.h>
#include "aruna.h"
#include <driver/mcpwm.h>

using namespace aruna;

drivers::comm::CommDriver *uart_driver;
drivers::control::ControlActuatorDriver* l293d_driver;
drivers::comm::CommDriver *rs485_driver;
aruna::log::channel_t *example_log;

extern "C" void app_main(void);
void testUART_task(void *param);
void start_comm();
void register_drivers();

void comm_test_task(void *arg) {
    comm::transmitpackage_t d;
    comm::channel_t testapp = {
            .port = 1,
    };
    //    TODO register channel needs redesign, all endpoint data as paramter. handeler should be used.
    example_log->info("comm register: 0x%X", comm::register_channel(&testapp));
    example_log->info("comm send: 0x%X", comm::send(&testapp, 0xAA, (uint8_t *) "precies 32 bytes aan data size!\n", 32, 0));
    example_log->info("comm send: 0x%X", comm::send(&testapp, 0xAE, (uint8_t *) "een overflow van data! ohh nee, wat gebeurt er nu?\n", 51, true));
	example_log->info("comm send: 0x%X", comm::send(&testapp, 0xDE, (uint8_t *) "My man!\n", 8, true));

    while (1) {
        if (testapp.receive(&d)) {
            example_log->info("howdy, partner!");
            example_log->info("data: '%s'", d.data_received);
            example_log->info("data_length: '%d'", d.data_lenght);
            example_log->info("from: '%d'", d.from_port);
            example_log->info("me: '%d'", d.to_port);
            testapp.receive_clear();
        }
    }
    vTaskDelete(NULL);
}


extern "C" void app_main(void) {
//    change to VERBOSE enable logging.
    log::set_max_level(log::level_t::NONE);
//
    example_log = new log::channel_t("aruna_example");
    log::set_level("comm", log::level_t::VERBOSE);
    log::set_level("aruna_example", log::level_t::VERBOSE);
	esp_log_level_set("UART", ESP_LOG_VERBOSE);
	example_log->info("hello world!");
    register_drivers();
    start_comm();

    control::start();
//	control::set_X_velocity(100, direction_t::PLUS);

//    test application

//		TODO task overflow?!
    xTaskCreate(comm_test_task, "comm test task", 2048, NULL, 0, NULL);
    xTaskCreate(aruna::blinky::start_blinky_task, "blinky_app", 2048, NULL, 0, NULL);

}

void register_drivers() {
//    TODO if there are more drivers, then this needs his own .cpp file

//  comm
// TODO error check
    uart_driver = new drivers::comm::UART();
    comm::register_candidate_driver(uart_driver);

	uart_config_t rs485_config = {
			.baud_rate = 5000000,
			.data_bits = UART_DATA_8_BITS,
			.parity = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
			.rx_flow_ctrl_thresh = 122,
			.use_ref_tick = false
	};
//	rs485_driver = new aruna::drivers::comm::UART((char*)"RS485",
//							UART_NUM_1,
//							23,
//							22,
//							18,
//							UART_PIN_NO_CHANGE,
//							rs485_config,
//							UART_MODE_RS485_HALF_DUPLEX,
//							256,
//							512);
//  comm::register_candidate_driver(rs485_driver);


//  Control
// TODO error check
    l293d_driver = new drivers::control::Pwm(control::axis_mask_t::X, 32, 33, MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0A, MCPWM0B);
    aruna::control::register_driver(l293d_driver);
}

void start_comm() {
    example_log->debug("comm start: 0x%X", comm::start(uart_driver));
    example_log->debug("comm status: 0x%X", comm::get_status());
    example_log->debug("comm driver name: %s", comm::getName());
    example_log->debug("comm speed: %i", comm::get_speed());
}
