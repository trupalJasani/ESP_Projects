#include "mpu6050.h"
#include "sensor_bus.h"
#include "esp_log.h"

static const char *MPU_TAG = "BSP_MPU6050";

// MPU6050 Register Addresses
#define MPU6050_PWR_MGMT_1_REG  0x6B
#define MPU6050_ACCEL_XOUT_H_REG 0x3B // Start of 14 bytes of sensor data

// Helper to swap bytes (MPU6050 is big-endian, ESP32 is little-endian)
static inline int16_t bytes_to_int16(uint8_t h, uint8_t l) {
    return (int16_t)((h << 8) | l);
}

esp_err_t bsp_mpu6050_init(void) {
    // 1. Wake up the MPU6050 (set PWR_MGMT_1 to 0)
    uint8_t wake_cmd = 0x00;
    esp_err_t err = bsp_i2c_write(MPU6050_SENSOR_ADDR, MPU6050_PWR_MGMT_1_REG, &wake_cmd, 1);
    if (err != ESP_OK) {
        ESP_LOGE(MPU_TAG, "Init failed to wake up MPU: %s", esp_err_to_name(err));
    }
    // TODO: Set Gyro/Accel ranges (e.g., 250 deg/s, 2g), DLPF, and Sample Rate Divider here.
    return err;
}

esp_err_t bsp_mpu6050_read_raw(int16_t accel_raw[3], int16_t gyro_raw[3]) {
    // Read 14 bytes starting from ACCEL_XOUT_H (0x3B)
    uint8_t data_buf[14];
    esp_err_t err = bsp_i2c_read(MPU6050_SENSOR_ADDR, MPU6050_ACCEL_XOUT_H_REG, data_buf, 14);

    if (err == ESP_OK) {
        // Accel Data (Index 0 to 5)
        accel_raw[0] = bytes_to_int16(data_buf[0], data_buf[1]);  // Ax
        accel_raw[1] = bytes_to_int16(data_buf[2], data_buf[3]);  // Ay
        accel_raw[2] = bytes_to_int16(data_buf[4], data_buf[5]);  // Az

        // Gyro Data (Index 8 to 13) - skipping temp data (Index 6, 7)
        gyro_raw[0] = bytes_to_int16(data_buf[8], data_buf[9]);   // Gx
        gyro_raw[1] = bytes_to_int16(data_buf[10], data_buf[11]); // Gy
        gyro_raw[2] = bytes_to_int16(data_buf[12], data_buf[13]); // Gz
    } else {
        ESP_LOGE(MPU_TAG, "I2C read failed: %s", esp_err_to_name(err));
    }
    
    return err;
}
