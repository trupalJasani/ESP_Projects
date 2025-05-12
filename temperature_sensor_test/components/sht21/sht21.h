#ifndef __SHT21_H__
#define __SHT21_H__

#include "driver/i2c.h"
#include "esp_check.h"
#include <assert.h>

#define SHT21_I2C_ADDRESS 0x40
#define SHT21_SDA_PIN GPIO_NUM_37
#define SHT21_SCL_PIN GPIO_NUM_36
#define SHT21_SPEED_STANDARD 400000
#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_TIMEOUT_MS 200
#define SHT21_DELAY_T_MEASUREMENT 85  // max delay for temp meas. in ms
#define SHT21_DELAY_RH_MEASUREMENT 29 // max delay for humidity meas. in ms
#define CRC_POLYNOMIAL 0x131          // P(x) = x^8 + x^5 + x^4 + 1 = 100110001
#define ER(x) ESP_RETURN_ON_ERROR(x, __FUNCTION__, #x)

typedef enum
{
    SHT21_CMD_TRIG_T_MEASUREMENT_HM = 0xE3,   // command trig. temp meas. hold master
    SHT21_CMD_TRIG_RH_MEASUREMENT_HM = 0xE5,  // command trig. humidity meas. hold master
    SHT21_CMD_TRIG_T_MEASUREMENT_NHM = 0xF3,  // command trig. temp meas. no hold master
    SHT21_CMD_TRIG_RH_MEASUREMENT_NHM = 0xF5, // command trig. humid. meas. no hold master
    SHT21_CMD_SOFT_RESET = 0xFE               // command soft reset
} sht21_command_t;

esp_err_t sht21_init(i2c_port_t i2c_num);
TickType_t get_delay_for_measurement(sht21_command_t command);
esp_err_t crc_checksum(uint8_t data_arr[], uint8_t data_len,
                       uint8_t checksum);
esp_err_t read_register(uint16_t *raw_data, sht21_command_t command);
esp_err_t sht21_get_temperature(float *dst);
esp_err_t sht21_get_humidity(float *dst);

#endif // __SHT21_H__