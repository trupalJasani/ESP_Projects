#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define PIR_SENSOR_GPIO GPIO_NUM_41

volatile uint8_t motion_sensor_state = 0;

static const char *TAG = "PIR_SENSOR";

// Interrupt handler (ISR)

static void IRAM_ATTR pir_sensor_isr_handler()
{
    int pir_level = gpio_get_level(PIR_SENSOR_GPIO);

    if (pir_level)
    {
        motion_sensor_state |= (1 << 0);
        motion_sensor_state |= (1 << 1);
    }
    else
    {
        motion_sensor_state &= ~(1 << 0);
        motion_sensor_state &= ~(1 << 1);
    }
}

void motion_detection_task(void *arg)
{
    while (1)
    {
        if (motion_sensor_state & (1 << 1))
        {
            ESP_EARLY_LOGI(TAG, "Motion Detected!");
        }
        else
        {
            ESP_EARLY_LOGI(TAG, "Motion Ended!");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    // Configure PIR sensor pin as input with interrupt on both rising and falling edge
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIR_SENSOR_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&io_conf);

    // Install GPIO ISR service
    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIR_SENSOR_GPIO, pir_sensor_isr_handler, NULL);

    ESP_LOGI(TAG, "Waiting for PIR sensor to stabilize...");

    vTaskDelay(pdMS_TO_TICKS(10000));

    ESP_LOGI(TAG, "PIR Sensor ready!");

    xTaskCreate(motion_detection_task, "motion_detection_task", 4096, NULL, 1, NULL);
}
