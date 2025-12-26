#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#define UART_NUM UART_NUM_1
#define TX_PIN   17
#define RX_PIN   16

void app_main(void)
{
    uart_config_t cfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_driver_install(UART_NUM, 2048, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &cfg);
    uart_set_pin(UART_NUM, TX_PIN, RX_PIN,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    uint8_t rx[256];

    while (1)
    {
        int len = uart_read_bytes(UART_NUM,
                                  rx,
                                  sizeof(rx) - 1,
                                  100 / portTICK_PERIOD_MS);

        if (len > 0)
        {
            rx[len] = '\0';
            printf("Received: %s", rx);
        }
    }
}
