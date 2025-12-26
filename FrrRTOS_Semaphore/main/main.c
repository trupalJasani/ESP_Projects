#include <stdio.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "driver/gpio.h"

TaskHandle_t BossTh;
TaskHandle_t EmployeeTh;
SemaphoreHandle_t EmployeeSignal;

#define BLINK_GPIO 2

static void Toggle_Led()
{
	gpio_set_level(BLINK_GPIO, 0);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	/* Blink on (output high) */
	gpio_set_level(BLINK_GPIO, 1);
	vTaskDelay(100 / portTICK_PERIOD_MS);
}

void BossThread()
{
	for (;;)
	{
		xSemaphoreGive(EmployeeSignal);
		printf("Boss Sent the signal\r\n");
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

void EmployeeThread()
{
	for (;;)
	{
		if (xSemaphoreTake(EmployeeSignal, portMAX_DELAY))
		{
			printf("Employee Received The signal\r\n");
			Toggle_Led();
		}
	}
}

void app_main(void)
{
	gpio_reset_pin(BLINK_GPIO);
	/* Set the GPIO as a push/pull output */
	gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

	/*Creating the binary semaphore  */
	EmployeeSignal = xSemaphoreCreateBinary();
	if (EmployeeSignal == NULL)
	{
		printf("Failed to Create Binary Semaphore\r\n");
		vSemaphoreDelete(EmployeeSignal);
	}

	xTaskCreate(BossThread, "Boss", 2048, NULL, 1, &BossTh);
	if (BossTh == NULL)
	{
		printf("Failed to create the BossThread\r\n");
		vTaskDelete(BossTh);
	}

	xTaskCreate(EmployeeThread, "employee", 2048, NULL, 2, &EmployeeTh);
	if (EmployeeTh == NULL)
	{
		printf("Failed to create EmployeeThread\r\n");
		vTaskDelete(EmployeeTh);
	}
}
