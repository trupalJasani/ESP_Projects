#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "driver/uart.h"
#include "driver/gpio.h"

#define GPS_TX_PIN GPIO_NUM_10
#define GPS_RX_PIN GPIO_NUM_9
#define GPS_UART_NUM UART_NUM_1
#define GPS_DATA_SIZE 1024

void gps_init()
{
    const uart_config_t gps_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_driver_install(GPS_UART_NUM, GPS_DATA_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(GPS_UART_NUM, &gps_config);
    uart_set_pin(GPS_UART_NUM, GPS_TX_PIN, GPS_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

float convert_to_decimal(const char *nmea_coord, const char *direction)
{
    float raw = atof(nmea_coord);
    int degrees = (int)(raw / 100);
    float minutes = raw - (degrees * 100);
    float decimal = degrees + (minutes / 60.0);
    if (direction[0] == 'S' || direction[0] == 'W')
        decimal *= -1;
    return decimal;
}

void parse_gnrmc(const char *line)
{
    char temp[128];
    strncpy(temp, line, sizeof(temp));
    char *token = strtok(temp, ",");
    int field = 0;

    char lat[16] = "", lat_dir[4] = "";
    char lon[16] = "", lon_dir[4] = "";

    while (token != NULL)
    {
        field++;
        if (field == 4)
            strcpy(lat, token);
        else if (field == 5)
            strcpy(lat_dir, token);
        else if (field == 6)
            strcpy(lon, token);
        else if (field == 7)
            strcpy(lon_dir, token);
        token = strtok(NULL, ",");
    }

    if (strlen(lat) && strlen(lat_dir) && strlen(lon) && strlen(lon_dir))
    {
        float lat_f = convert_to_decimal(lat, lat_dir);
        float lon_f = convert_to_decimal(lon, lon_dir);
        printf("Latitude: %.15f %s\r\nLongitude: %.15f %s\r\n", lat_f, lat_dir, lon_f, lon_dir);
    }
}

void app_main(void)
{
    gps_init();
    int len;
    uint8_t data[GPS_DATA_SIZE];
    char *line;

    while (1)
    {
        len = uart_read_bytes(GPS_UART_NUM, data, GPS_DATA_SIZE - 1, pdMS_TO_TICKS(1000));
        if (len > 0)
        {
            data[len] = 0;
            // Tokenize data by line
            line = strtok((char *)data, "\n");
            while (line != NULL)
            {
                if (strstr(line, "$GNRMC"))
                {
                    parse_gnrmc(line);
                }
                line = strtok(NULL, "\n");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
