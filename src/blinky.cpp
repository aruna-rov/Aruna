//
// Created by noeel on 23-3-19.
//

#include "aruna/blinky.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <aruna/comm.h>
#include <aruna/blinky.h>
#include <esp_log.h>

#define LED_GPIO_PIN GPIO_NUM_2
namespace aruna {
    namespace blinky {

        void start_blinky_task(void *arg) {
            ESP_LOGV("BLINK", "started!");
            QueueHandle_t handler;
            comm::transmitpackage_t tp;
            comm::channel_t blinky_comm_channel = {
                    .port = 4,
                    .priority = 2,
                    .handeler = &handler
            };
            comm::register_channel(&blinky_comm_channel);
            gpio_pad_select_gpio(LED_GPIO_PIN);
            ESP_ERROR_CHECK(gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT));
            set_led(LED_ON);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            set_led(LED_OFF);

            while (1) {
                if (xQueueReceive(handler, (void *) &tp, (portTickType) portMAX_DELAY)) {
                    if (tp.data_lenght > 3) break;
                    switch (tp.data_received[0]) {
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
                            t = tp.data_received[1] * 100;
                            for (int i = 0; i < tp.data_received[2]; ++i) {
//                                TODO if the loop is long then a new message wont get handled
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
            }
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
            ESP_ERROR_CHECK(gpio_set_level(LED_GPIO_PIN, led_status))
        }

    }
}