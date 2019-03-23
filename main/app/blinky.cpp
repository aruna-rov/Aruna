//
// Created by noeel on 23-3-19.
//

#include "app/blinky.h"
#include <freertos/task.h>
#include <driver/gpio.h>
#include <app/Com.h>
#include <app/blinky.h>
#include <sdkconfig.h>

#define LED_GPIO_PIN GPIO_NUM_2

void start_blinky_task(void* arg) {
    ESP_LOGV("BLINK", "started!");
    com_channel_t blinky_com_channel = {
            .port = 2,
            .priority = 2,
            .handeler = &blinky_com_handeler
    };
    COM.register_channel(&blinky_com_channel);
    gpio_pad_select_gpio(LED_GPIO_PIN);
    ESP_ERROR_CHECK(gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT));
    set_led(LED_ON);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    set_led(LED_OFF);

    vTaskDelete(NULL);
}

void set_led(LED_STATUS ls) {
    static int led_status;
    switch (ls) {
        case LED_ON:
            led_status = 1;
            break;
        case LED_OFF:
            led_status = 0;
            break;
        case LED_TOGGLE:
            led_status = !led_status;
            break;
    }
    ESP_LOGV("BLINK", "blinked: %d", led_status);
    ESP_ERROR_CHECK(gpio_set_level(LED_GPIO_PIN, led_status))
}

void blinky_com_handeler(com_transmitpackage_t tp) {
    if (tp.data_lenght > 3) return;
    switch (tp.data[0]) {
        case 0x00:
            set_led(LED_OFF);
            break;
        case 0x01:
            set_led(LED_ON);
            break;
        case 0x02:
            set_led(LED_TOGGLE);
            break;
        case 0x03:
            int t;
            t = tp.data[1] * 100;
            for (int i = 0; i < tp.data[2]; ++i) {
                set_led(LED_TOGGLE);
                vTaskDelay(t / portTICK_PERIOD_MS);
                set_led(LED_TOGGLE);
                vTaskDelay(t / portTICK_PERIOD_MS);
            }
            set_led(LED_OFF);
            break;
        default:
            return;
    }
}
