#include <stdio.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "sht21.h"

TaskHandle_t sht21=NULL;

void sht21_task(void *pvParameters)
{
    sht21_init(I2C_NUM_0);

    float temperature;
    float humidity;

    while (1)
    {
        sht21_get_temperature(&temperature);
        printf("Temperature: %.2f°C\n", temperature);

        sht21_get_humidity(&humidity);
        printf("Humidity: %.2f%%\n", humidity);

        vTaskDelay(pdMS_TO_TICKS(200)); // Delay 2 seconds
    }
}

void app_main(void)
{
    xTaskCreate(sht21_task, "sht21_task", 4096, NULL, 5, &sht21);
}
