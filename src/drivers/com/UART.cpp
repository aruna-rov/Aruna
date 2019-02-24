//
// Created by noeel on 16-1-19.
//

#include <stdio.h>
// TODO dubbele imports weghalen
#include "drivers/com/UART.h"
#include <drivers/Com.h>
#include <drivers/com/UART.h>
#include <esp_log.h>

static intr_handle_t handle_console;

com_err UART::transmit(com_transmitpackage_t package) {
//    printf(package);
    printf("hello world!");
//    TODO error handeling
    return COM_OK;
}

unsigned int UART::getSpeed() {
//    TODO get braudrate dynamicly
    return (unsigned int) BROAD_RATE / 8;
}

com_link_t UART::getLinkType() {
    return COM_WIRED;
}

bool UART::isEndpointConnected() {
//    TODO endpoint detection
    return true;
}

char *UART::getName() {
    return (char *) "UART";
}

void IRAM_ATTR UART::uart_intr_handle(void *arg) {
    uint8_t rxbuf[BUF_SIZE];     // Receive buffer to collect incoming data
    uint32_t rx_fifo_len;
    size_t i = 0;     // Register to collect data length

//    TODO always reads from UART0 now. Should use `UART_NUM` variable.
    rx_fifo_len = UART0.status.rxfifo_cnt; // read number of bytes in UART buffer

    while (rx_fifo_len) {
        rxbuf[i++] = UART0.fifo.rw_byte; // read all bytes
        rx_fifo_len--;
    }
    // after reading bytes from buffer clear UART interrupt status
    uart_clear_intr_status(UART_NUM_0, UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_TOUT_INT_CLR);

    // a test code or debug code to indicate UART receives successfully,
    // you can redirect received byte as echo also
    uart_write_bytes(UART_NUM_0, (const char *) rxbuf, i);
}

com_err UART::start() {
    ESP_LOGD("UART", "START");
//    TODO logging zou moeten worden uitgeschakeld op de target UART.

    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
            .baud_rate = BROAD_RATE,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
//                TODO krijg nu een warning dat niet alles geconfugureerd is.
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, RTS_PIN, CTS_PIN));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));

    ESP_ERROR_CHECK(uart_isr_free(UART_NUM));


    ESP_ERROR_CHECK(uart_isr_register(UART_NUM, UART::uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console));
    ESP_ERROR_CHECK(uart_enable_rx_intr(UART_NUM));
    return COM_OK;
}

com_err UART::stop() {
    ESP_LOGD("UART", "STOP");
//    TODO flush queue
    return (uart_isr_free(UART_NUM) || uart_driver_delete(UART_NUM)) ? COM_ERR_HARDWARE : COM_OK;
}

void UART::intr_incoming_connection() {

}



//com_err UART::registerDriver() {
//    ESP_LOGD("UART", "registering driver!");6
//    ComDriver * driver = this;
////    TODO error handeling.
//    return this->registerd = COM.register_candidate_driver(driver);
//};

