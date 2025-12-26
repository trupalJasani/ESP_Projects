#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void task1(void *pvParameters)
{
	while (1)
	{
		vTaskDelay(10000 / portTICK_PERIOD_MS);
		printf("Task 1 running\n");
		vTaskDelay(10000 / portTICK_PERIOD_MS);
	}
}
void task2()
{
	while (1)
	{
		printf("Task 2 is running\r\n");
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}
void app_main()
{
	xTaskCreate(task1, "Task 1", 2048, NULL, 5, NULL);
	xTaskCreate(task2, "Task 2", 2048, NULL, 6, NULL);
}
