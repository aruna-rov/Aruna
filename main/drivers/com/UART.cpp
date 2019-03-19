//
// Created by noeel on 16-1-19.
//

#include <stdio.h>
#include "UART.h"
#include "Com.h"
#include "esp_log.h"

static QueueHandle_t uart_queue;
static bool installed = false;
const static char TAG[] = "UART";


com_err UART::transmit(com_transmitpackage_t package) {
    ESP_LOGV(TAG, "sending...");
    com_bin_t bin_data;
    com_transmitpackage_t::transmitpackage_to_binary(package, bin_data);
    ESP_LOG_BUFFER_HEXDUMP(TAG, bin_data, sizeof(bin_data), ESP_LOG_VERBOSE);
    if (uart_write_bytes(UART_NUM, (const char *) bin_data, strlen((char *) bin_data)) == strlen((char *)bin_data))
        return COM_OK;
    else
        return COM_ERR_HARDWARE;
}

unsigned int UART::getSpeed() {
//    TODO get braudrate dynamicly
//  uart_get_baudrate()
    return (unsigned int) (BROAD_RATE / 8);
}

com_link_t UART::getLinkType() {
    return COM_WIRED;
}

bool UART::isEndpointConnected() {
//    TODO endpoint detection
    return true;
}

char *UART::getName() {
    return (char *) TAG;
}

com_err UART::start() {
    ESP_LOGD(TAG, "START");
//    TODO logging zou moeten worden uitgeschakeld op de target UART.

    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    if (!installed) {
//        uart fails when initialized two times.
        uart_config_t uart_config = {
                .baud_rate = BROAD_RATE,
                .data_bits = UART_DATA_8_BITS,
                .parity    = UART_PARITY_DISABLE,
                .stop_bits = UART_STOP_BITS_1,
                .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                .rx_flow_ctrl_thresh = 122,
                .use_ref_tick = false
        };
        ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
        ESP_ERROR_CHECK(uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, RTS_PIN, CTS_PIN));
        ESP_ERROR_CHECK(uart_driver_install(UART_NUM, RX_BUF_SIZE, TX_BUF_SIZE, 20, &uart_queue, 0));
        installed = true;
    }
    xTaskCreate(UART::handle_rx_task, "handle_rx_task", 2048, NULL, 12, NULL);
    return COM_OK;
}

com_err UART::stop() {
    ESP_LOGD(TAG, "STOP");
//    TODO flush queue
//    TODO doesn't propabily delete driver.
//    return (
//            (uart_disable_rx_intr(UART_NUM) ||
////    "uart: UART driver already installed" error on reinstall.
////            uart_driver_delete(UART_NUM) ||
//            uart_isr_free(UART_NUM)
//            ) !=
//           ESP_OK) ? COM_ERR_HARDWARE : COM_OK;
    return COM_OK;
}

void UART::handle_rx_task(void *arg) {
    uart_event_t event;
    uint8_t* dtmp = (uint8_t*) malloc(RX_BUF_SIZE);
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(uart_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
            bzero(dtmp, RX_BUF_SIZE);
            ESP_LOGV(TAG, "uart[%d] event:", UART_NUM);
            switch(event.type) {
                //Event of UART receving data
                /*We'd better handler data event fast, there would be much more data events than
                other types of events. If we take too much time on data event, the queue might
                be full.*/
                case UART_DATA:
                    ESP_LOGV(TAG, "[UART DATA]: %d", event.size);
                    uart_read_bytes(UART_NUM, dtmp, event.size, portMAX_DELAY);
                    ESP_LOGV(TAG, "[DATA EVT]:");
                    uart_write_bytes(UART_NUM, (const char*) dtmp, event.size);
                    //    TODO binnenkomende bericht afhandelen.
                    //    com_bin_t *bin_data = rxbuf;
                    //    COM.incoming_connection()
                    break;
                    //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGW(TAG, "hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(UART_NUM);
                    xQueueReset(uart_queue);
                    break;
                    //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGE(TAG, "ring buffer full");
                    // If buffer full happened, you should consider encreasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(UART_NUM);
                    xQueueReset(uart_queue);
                    break;
                    //Event of UART RX break detected
                case UART_BREAK:
                    ESP_LOGI(TAG, "uart rx break");
                    break;
                    //Event of UART parity check error
                case UART_PARITY_ERR:
                    ESP_LOGW(TAG, "uart parity error");
                    break;
                    //Event of UART frame error
                case UART_FRAME_ERR:
                    ESP_LOGW(TAG, "uart frame error");
                    break;
                    //UART_PATTERN_DET
                case UART_PATTERN_DET:
                    //Others
                default:
                    ESP_LOGE(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}




//com_err UART::registerDriver() {
//    ESP_LOGD(TAG, "registering driver!");6
//    ComDriver * driver = this;
////    TODO error handeling.
//    return this->registerd = COM.register_candidate_driver(driver);
//};

