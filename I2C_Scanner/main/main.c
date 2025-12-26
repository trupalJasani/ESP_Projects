#include <stdio.h>
#include <string.h>
#include <time.h>      // Needed for struct tm
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_SDA         4
#define I2C_SCL         5
#define I2C_PORT        I2C_NUM_0
#define I2C_FREQ        400000

#define LCD_ADDR        0x27
#define LCD_COLS        16
#define LCD_ROWS        2

#define LCD_BACKLIGHT   0x08
#define LCD_ENABLE      0x04
#define LCD_CMD         0x00
#define LCD_DATA        0x01

#define DS3231_ADDR     0x68

static const char *TAG = "ESP32S3_CLOCK";

// ---------------- LCD FUNCTIONS ----------------
void lcd_i2c_send(uint8_t val) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, LCD_ADDR << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, val | LCD_BACKLIGHT, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, 50 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

void lcd_write_nibble(uint8_t nibble, uint8_t mode) {
    uint8_t data = nibble | mode | LCD_BACKLIGHT;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, LCD_ADDR << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, data | LCD_ENABLE, true);
    i2c_master_write_byte(cmd, data & ~LCD_ENABLE, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, 50 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

void lcd_send_cmd(uint8_t cmd) {
    lcd_write_nibble(cmd & 0xF0, LCD_CMD);
    lcd_write_nibble((cmd << 4) & 0xF0, LCD_CMD);
}

void lcd_send_data(uint8_t data) {
    lcd_write_nibble(data & 0xF0, LCD_DATA);
    lcd_write_nibble((data << 4) & 0xF0, LCD_DATA);
}

void lcd_clear() {
    lcd_send_cmd(0x01);
    vTaskDelay(5 / portTICK_PERIOD_MS);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40};
    lcd_send_cmd(0x80 | (col + row_offsets[row]));
}

void lcd_print(const char *str) {
    while (*str) lcd_send_data(*str++);
}

void lcd_init() {
    vTaskDelay(50 / portTICK_PERIOD_MS);
    lcd_write_nibble(0x30, LCD_CMD);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    lcd_write_nibble(0x30, LCD_CMD);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    lcd_write_nibble(0x20, LCD_CMD);

    lcd_send_cmd(0x28); // 4bit, 2 lines
    lcd_send_cmd(0x08); // display off
    lcd_send_cmd(0x01); // clear
    vTaskDelay(5 / portTICK_PERIOD_MS);
    lcd_send_cmd(0x06); // entry mode
    lcd_send_cmd(0x0C); // display on
}

// ---------------- DS3231 RTC FUNCTIONS ----------------
uint8_t bcd2dec(uint8_t val) {
    return (val >> 4) * 10 + (val & 0x0F);
}

esp_err_t ds3231_read(struct tm *timeinfo) {
    uint8_t data[7] = {0};
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (DS3231_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x00, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (DS3231_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 7, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, 100 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) return ret;

    timeinfo->tm_sec  = bcd2dec(data[0]);
    timeinfo->tm_min  = bcd2dec(data[1]);
    timeinfo->tm_hour = bcd2dec(data[2]);
    timeinfo->tm_mday = bcd2dec(data[4]);
    timeinfo->tm_mon  = bcd2dec(data[5]) - 1;
    timeinfo->tm_year = bcd2dec(data[6]) + 100; // years since 1900
    return ESP_OK;
}

// ---------------- I2C INIT ----------------
void i2c_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA,
        .scl_io_num = I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0));
}

// ---------------- MAIN APP ----------------
void app_main() {
    i2c_init();
    lcd_init();
    lcd_clear();
    lcd_print("ESP32-S3 Clock");
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    lcd_clear();

    while (1) {
        struct tm timeinfo = {0}; // initialize all fields to zero
        if (ds3231_read(&timeinfo) == ESP_OK) {
            char line1[32], line2[32];
            int year = timeinfo.tm_year + 1900;

            snprintf(line1, sizeof(line1), "%02d/%02d/%04d",
                     timeinfo.tm_mday,
                     timeinfo.tm_mon + 1,
                     year);

            snprintf(line2, sizeof(line2), "%02d:%02d:%02d",
                     timeinfo.tm_hour,
                     timeinfo.tm_min,
                     timeinfo.tm_sec);

            lcd_clear();
            lcd_set_cursor(0,0);
            lcd_print(line1);
            lcd_set_cursor(0,1);
            lcd_print(line2);
        } else {
            lcd_clear();
            lcd_set_cursor(0,0);
            lcd_print("RTC error!");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
