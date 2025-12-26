#include "sht21.h"
#include "sensor_bus.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"

static const char *SHT_TAG = "BSP_SHT21";

// SHT21 Command for Temperature (No Hold Master mode)
#define SHT21_TEMP_NO_HOLD_CMD      0xF3

esp_err_t bsp_sht21_read_raw(uint16_t *temp_raw) {
    uint8_t cmd = SHT21_TEMP_NO_HOLD_CMD;
    uint8_t read_buf[2];
    esp_err_t err;

    // 1. Send temperature measurement command
    i2c_cmd_handle_t cmd_link = i2c_cmd_link_create();
    i2c_master_start(cmd_link);
    i2c_master_write_byte(cmd_link, (SHT21_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_link, cmd, true);
    i2c_master_stop(cmd_link);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_link, 50 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd_link);

    if (err != ESP_OK) return err;

    // 2. Wait for measurement to complete (100ms is safe)
    vTaskDelay(pdMS_TO_TICKS(100));

    // 3. Read the 2 bytes of data
    cmd_link = i2c_cmd_link_create();
    i2c_master_start(cmd_link);
    i2c_master_write_byte(cmd_link, (SHT21_SENSOR_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd_link, &read_buf[0], I2C_MASTER_ACK);
    i2c_master_read_byte(cmd_link, &read_buf[1], I2C_MASTER_NACK);
    i2c_master_stop(cmd_link);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_link, 100 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd_link);

    if (err == ESP_OK) {
        // Combine MSB and LSB, and clear the status bits (last 2 bits)
        *temp_raw = ((uint16_t)read_buf[0] << 8) | (read_buf[1] & 0xFC);
    } else {
        ESP_LOGE(SHT_TAG, "I2C read failed: %s", esp_err_to_name(err));
    }

    return err;
}
