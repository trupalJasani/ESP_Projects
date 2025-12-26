#include "mpu6500.h"
#include "esp_log.h"

#define TAG "MPU6500"

static esp_err_t mpu6500_write_byte(i2c_port_t i2c_num, uint8_t reg_addr, uint8_t data)
{
    uint8_t buf[2] = {reg_addr, data};
    return i2c_master_write_to_device(i2c_num, MPU6500_I2C_ADDR, buf, 2, pdMS_TO_TICKS(100));
}

static esp_err_t mpu6500_read_bytes(i2c_port_t i2c_num, uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(i2c_num, MPU6500_I2C_ADDR, &reg_addr, 1, data, len, pdMS_TO_TICKS(100));
}
void mpu6500_convert(const mpu6500_raw_data_t *raw, mpu6500_data_t *converted)
{
    const float ACCEL_SENSITIVITY = 16384.0; // for ±2g
    const float GYRO_SENSITIVITY = 131.0;    // for ±250 °/s
    const float TEMP_SENSITIVITY = 333.87;
    const float TEMP_OFFSET = 21.0;

    converted->accel_x_g = raw->accel_x / ACCEL_SENSITIVITY;
    converted->accel_y_g = raw->accel_y / ACCEL_SENSITIVITY;
    converted->accel_z_g = raw->accel_z / ACCEL_SENSITIVITY;

    converted->gyro_x_dps = raw->gyro_x / GYRO_SENSITIVITY;
    converted->gyro_y_dps = raw->gyro_y / GYRO_SENSITIVITY;
    converted->gyro_z_dps = raw->gyro_z / GYRO_SENSITIVITY;

    converted->temperature_c = (raw->temperature / TEMP_SENSITIVITY) + TEMP_OFFSET;
}

esp_err_t mpu6500_init(i2c_port_t i2c_num)
{
    uint8_t id = 0;
    esp_err_t ret = mpu6500_read_bytes(i2c_num, MPU6500_WHO_AM_I, &id, 1);
    if (ret != ESP_OK || id != MPU6500_WHO_AM_I_ID)
    {
        ESP_LOGE(TAG, "WHO_AM_I failed! Read: 0x%02X", id);
        return ESP_FAIL;
    }

    mpu6500_write_byte(i2c_num, MPU6500_PWR_MGMT_1, 0x00);   // Wake ups
    mpu6500_write_byte(i2c_num, MPU6500_SMPLRT_DIV, 0x07);   // Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
    mpu6500_write_byte(i2c_num, MPU6500_CONFIG, 0x06);       // DLPF 5Hz
    mpu6500_write_byte(i2c_num, MPU6500_GYRO_CONFIG, 0x00);  // ±250 °/s
    mpu6500_write_byte(i2c_num, MPU6500_ACCEL_CONFIG, 0x00); // ±2g

    ESP_LOGI(TAG, "MPU6500 initialized.");
    return ESP_OK;
}

esp_err_t mpu6500_read_raw(i2c_port_t i2c_num, mpu6500_raw_data_t *data)
{
    uint8_t buf[14];
    esp_err_t ret = mpu6500_read_bytes(i2c_num, MPU6500_ACCEL_XOUT_H, buf, 14);
    if (ret != ESP_OK)
        return ret;

    data->accel_x = (int16_t)((buf[0] << 8) | buf[1]);
    data->accel_y = (int16_t)((buf[2] << 8) | buf[3]);
    data->accel_z = (int16_t)((buf[4] << 8) | buf[5]);
    data->temperature = (int16_t)((buf[6] << 8) | buf[7]);
    data->gyro_x = (int16_t)((buf[8] << 8) | buf[9]);
    data->gyro_y = (int16_t)((buf[10] << 8) | buf[11]);
    data->gyro_z = (int16_t)((buf[12] << 8) | buf[13]);

    return ESP_OK;
}
