#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

QueueHandle_t queue;

void sender_task(void *param) {
    int count = 0;
    while (1) {
        printf("Sender: sending %d\n", count);
        xQueueSend(queue, &count, portMAX_DELAY);
        count++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void receiver_task(void *param) {
    int received_value;
    while (1) {
        if (xQueueReceive(queue, &received_value, portMAX_DELAY)) {
            printf("Receiver: received %d\n", received_value);
        }
    }
}

void app_main(void) {
    // Create a queue to hold 10 integers
    queue = xQueueCreate(10, sizeof(int));
    if (queue == NULL) {
        printf("Queue creation failed!\n");
        return;
    }

    xTaskCreate(sender_task, "sender_task", 2048, NULL, 1, NULL);
    xTaskCreate(receiver_task, "receiver_task", 2048, NULL, 1, NULL);
}
