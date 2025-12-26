#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define DELAY 1000
#define SUCESS 1
#define QUEUE_SIZE 10

QueueHandle_t queue_1;
TaskHandle_t task1_handler, task2_handler;

static void task_1()
{
    uint8_t count = 0;
    while (1)
    {
        count++;
        xQueueSend(queue_1, &count, portMAX_DELAY);
        printf("sent = %d\r\n ", count);
        vTaskDelay(DELAY / portTICK_PERIOD_MS);
    }
}
static void task_2()
{
    uint8_t receive = 0;
    while (1)
    {
        if (xQueueReceive(queue_1, &receive, portMAX_DELAY))
        {
            printf("received  = %d\r\n",receive);
        }
        vTaskDelay(DELAY / portTICK_PERIOD_MS);
    }
}
void app_main(void)
{
    BaseType_t xreturn;
    queue_1 = xQueueCreate(QUEUE_SIZE, sizeof(uint8_t));

    xreturn = xTaskCreate(task_1, "Task1", 2048, NULL, 5, &task1_handler);
    if (xreturn != SUCESS)
    {
        printf("Task 1 is not crrated \r\n");
    }
    xreturn = xTaskCreate(task_2, "Task2", 2048, NULL, 2, &task2_handler);
    if (xreturn != SUCESS)
    {
        printf("Task 2 is not crrated \r\n");
    }
}