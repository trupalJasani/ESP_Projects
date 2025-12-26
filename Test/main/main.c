#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "led_strip.h"

#define TTP223_GPIO GPIO_NUM_4
#define LED_STRIP_GPIO GPIO_NUM_48

static led_strip_handle_t led_strip;
TaskHandle_t t_1 = NULL;
TaskHandle_t t_2 = NULL;
int i;
void task_1()
{

    while (1)
    {
        int level = gpio_get_level(TTP223_GPIO);
        if (level == 1)
        {
            led_strip_set_pixel(led_strip, 0, 255, 0, 0); // Red
            led_strip_refresh(led_strip);                 // Refresh the LED strip
        }
        else
        {
            led_strip_clear(led_strip);
        }
        printf("Task 1 is running  \r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_2()
{

    while (1)
    {

        printf("Task 2 is running  \r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void app_main(void)
{
    // Configure touch input
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << TTP223_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);

    // LED strip config (RMT-based)
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO,
        .max_leds = 1,
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    led_strip_clear(led_strip);

    BaseType_t xReturn;

    xReturn = xTaskCreate(task_1, "task 1", 4096, NULL, 1, &t_1);
    xReturn = xTaskCreate(task_2, "task 2", 4096, NULL, 2, &t_2);
}
