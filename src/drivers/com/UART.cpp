//
// Created by noeel on 16-1-19.
//

#include <stdio.h>
#include <aruna/Com.h>
#include "aruna/drivers/com/UART.h"
#include "esp_log.h"
namespace aruna { namespace drivers { namespace com {
UART::UART(){}
UART::UART(char *TAG,
           uart_port_t UART_NUM,
           int TXD_PIN,
           int RXD_PIN,
           int RTS_PIN,
           int CTS_PIN,
           uart_config_t UART_CONFIG,
           uart_mode_t UART_MODE,
           unsigned int TX_BUF_SIZE,
           unsigned int RX_BUF_SIZE):
        TAG(TAG),
        UART_NUM(UART_NUM),
        TXD_PIN(TXD_PIN),
        RXD_PIN(RXD_PIN),
        RTS_PIN(RTS_PIN),
        CTS_PIN(CTS_PIN),
        UART_CONFIG(UART_CONFIG),
        UART_MODE(UART_MODE),
        TX_BUF_SIZE(TX_BUF_SIZE),
        RX_BUF_SIZE(RX_BUF_SIZE){}

Com::err_t UART::transmit(uint8_t *package, uint8_t package_size) {
    ESP_LOGV(TAG, "sending...");
    ESP_LOG_BUFFER_HEXDUMP(TAG, package, package_size, ESP_LOG_VERBOSE);
//    transmit

//	TODO transmit() is not thread safe, it should work with a xqueue where data get pushed thru from Com to ComDriver
    if (uart_write_bytes(UART_NUM,
                         (const char *) package,
                         package_size)
        == (package_size)) {
		return Com::err_t::OK;
	}

    else {
		return Com::err_t::HARDWARE;
	}
}

unsigned int UART::getSpeed() {
    uint32_t br = 0;
    uart_get_baudrate(UART_NUM, &br);
//    TODO speed should be in bits not in bytes
    return (unsigned int) (br / 8);
}

Com::link_t UART::getLinkType() {
    return Com::link_t::WIRED;
}

bool UART::isEndpointConnected() {
//    TODO endpoint detection
    return true;
}

char *UART::getName() {
    return (char *) TAG;
}

Com::err_t UART::start() {
    ESP_LOGD(TAG, "START");
//    TODO logging zou moeten worden uitgeschakeld op de target UART.

    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    if (!installed) {
//        set config
        ESP_ERROR_CHECK(uart_param_config(UART_NUM, &UART_CONFIG));
//        set pins
        ESP_ERROR_CHECK(uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, RTS_PIN, CTS_PIN));
//        install driver (driver can only be installed once, even if you uart_driver_delete is used)
        ESP_ERROR_CHECK(uart_driver_install(UART_NUM, RX_BUF_SIZE, TX_BUF_SIZE, 20, &uart_queue, 0));
//        Set correct mode
        ESP_ERROR_CHECK(uart_set_mode(UART_NUM, UART_MODE));
        installed = true;
    }
//    task to handle incomming messages.
    if (uart_rx_handle == nullptr)
//        TODO set name acording to TAG
		xTaskCreate(UART::handle_rx_task, "handle_rx_uart", 2048, this, 12, &uart_rx_handle);
    return Com::err_t::OK;
}

Com::err_t UART::stop() {
    ESP_LOGD(TAG, "STOP");
//    TODO flush tx queue
//  flush rx queue
    xQueueReset(uart_queue);
    esp_err_t ufi = uart_flush_input(UART_NUM);
    if (ufi != ESP_OK)
        ESP_LOGE(TAG, "error on flush: %s", esp_err_to_name(ufi));
/*
 *      new driver cannot be installed even if uart_delete_driver is called.
 *      this causes an issue in driver selection because the winning driver gets restarted
 */

//    esp_err_t udd = uart_driver_delete(UART_NUM);
//    if (udd != ESP_OK)
//        ESP_LOGE(TAG, "error on driver delete: %s", esp_err_to_name(udd));

//  delete task
    vTaskDelete(uart_rx_handle);
    uart_rx_handle = nullptr;
    return (ufi) != ESP_OK ? Com::err_t::HARDWARE : Com::err_t::OK;
}

void UART::handle_rx_task(void *__this) {
    uart_event_t event;

    UART *_this = static_cast<UART*>(__this);
//    smaller size for dtmp results in an stack overflow.
    uint8_t *dtmp = (uint8_t *) malloc(_this->RX_BUF_SIZE);
    int read;
    for (;;) {
        //Waiting for UART event.
        if (xQueueReceive(_this->uart_queue, (void *) &event, (portTickType) portMAX_DELAY)) {
            bzero(dtmp, _this->RX_BUF_SIZE);
            ESP_LOGV(_this->TAG, "uart[%d] event:", _this->UART_NUM);
            switch (event.type) {
                //Event of UART receving data
                /*We'd better handler data event fast, there would be much more data events than
                other types of events. If we take too much time on data event, the queue might
                be full.*/
                case UART_DATA:
//                    TODO UART_DATA event gets triggerd without any data being avaliable
                    read = uart_read_bytes(_this->UART_NUM, dtmp, 1, 4);
					if (read <= 0) break;
					read = uart_read_bytes(_this->UART_NUM, &dtmp[1], (dtmp[0] -1 ), 10);
//                    stop if read fails
					if (read != (dtmp[0] -1)) {
						ESP_LOGV(_this->TAG, "could not read all bytes");
						break;
					}
					ESP_LOGV(_this->TAG, "incoming data[%d]:", read);
//                    if the data now contains a 0x0 then datalength will be set at that byte.
                    ESP_LOG_BUFFER_HEXDUMP(_this->TAG, dtmp, dtmp[0], ESP_LOG_VERBOSE);
//                    convert binary to transmitpackage and alert COM of an incomming connection.
                    if (COM.incoming_connection(dtmp, dtmp[0]) != Com::err_t::OK) {
                        ESP_LOGV(_this->TAG, "protocol error");
                    }
                    break;
                    //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGW(_this->TAG, "hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(_this->UART_NUM);
                    xQueueReset(_this->uart_queue);
                    break;
                    //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGE(_this->TAG, "ring buffer full");
                    // If buffer full happened, you should consider encreasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(_this->UART_NUM);
                    xQueueReset(_this->uart_queue);
                    break;
                    //Event of UART RX break detected
                case UART_BREAK:
                    ESP_LOGI(_this->TAG, "uart rx break");
                    break;
                    //Event of UART parity check error
                case UART_PARITY_ERR:
                    ESP_LOGW(_this->TAG, "uart parity error");
                    break;
                    //Event of UART frame error
                case UART_FRAME_ERR:
                    ESP_LOGW(_this->TAG, "uart frame error");
                    break;
                    //UART_PATTERN_DET
                case UART_PATTERN_DET:
                    //Others
                default:
                    ESP_LOGE(_this->TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}
}}}
