// mpu6500.h

#ifndef MPU6500_H
#define MPU6500_H

#include "driver/i2c.h"
#include "esp_err.h"
#include <stdint.h>
#include "mpu6500_reg.h"

typedef struct
{
    float accel_x_g;
    float accel_y_g;
    float accel_z_g;
    float temperature_c;
    float gyro_x_dps;
    float gyro_y_dps;
    float gyro_z_dps;
} mpu6500_data_t;

typedef struct
{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t temperature;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} mpu6500_raw_data_t;

esp_err_t mpu6500_init(i2c_port_t i2c_num);
void mpu6500_convert(const mpu6500_raw_data_t *raw, mpu6500_data_t *converted);
esp_err_t mpu6500_read_raw(i2c_port_t i2c_num, mpu6500_raw_data_t *data);

#endif // MPU6500_H
