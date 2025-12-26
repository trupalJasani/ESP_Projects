// mpu6050.h

#ifndef MPU6050_H
#define MPU6050_H

#include "driver/i2c.h"
#include "esp_err.h"
#include <stdint.h>
#include "mpu6050_reg.h"

typedef struct
{
    float accel_x_g;
    float accel_y_g;
    float accel_z_g;
    float temperature_c;
    float gyro_x_dps;
    float gyro_y_dps;
    float gyro_z_dps;
} MPU6050_data_t;

typedef struct
{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t temperature;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} MPU6050_raw_data_t;

esp_err_t MPU6050_init(i2c_port_t i2c_num);
void MPU6050_convert(const MPU6050_raw_data_t *raw, MPU6050_data_t *converted);
esp_err_t MPU6050_read_raw(i2c_port_t i2c_num, MPU6050_raw_data_t *data);

#endif // MPU6050_H