#ifndef BSP_MPU6050_H
#define BSP_MPU6050_H

#include "esp_err.h"
#include <stdint.h>

/**
 * @brief Initializes the MPU6050 sensor (wakes it up, sets sample rate, ranges).
 * @return ESP_OK on success.
 */
esp_err_t bsp_mpu6050_init(void);

/**
 * @brief Reads raw sensor data from MPU6050.
 * @param accel_raw Output array [Ax, Ay, Az]
 * @param gyro_raw Output array [Gx, Gy, Gz]
 * @return ESP_OK on success.
 */
esp_err_t bsp_mpu6050_read_raw(int16_t accel_raw[3], int16_t gyro_raw[3]);

#endif // BSP_MPU6050_H
