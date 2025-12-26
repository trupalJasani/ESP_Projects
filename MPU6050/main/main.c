// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/i2c.h"
// #include "mpu6050.h"

// #define I2C_MASTER_SCL_IO 36
// #define I2C_MASTER_SDA_IO 37
// #define I2C_MASTER_NUM I2C_NUM_0
// #define I2C_MASTER_FREQ_HZ 400000
// #define I2C_MASTER_TX_BUF_DISABLE 0
// #define I2C_MASTER_RX_BUF_DISABLE 0

// #define ACCEL_THRESHOLD 0.3f // for tilt direction
// #define GYRO_THRESHOLD 30.0f // for rotation detection

// static void i2c_master_init()
// {
//     i2c_config_t conf = {
//         .mode = I2C_MODE_MASTER,
//         .sda_io_num = I2C_MASTER_SDA_IO,
//         .sda_pullup_en = GPIO_PULLUP_ENABLE,
//         .scl_io_num = I2C_MASTER_SCL_IO,
//         .scl_pullup_en = GPIO_PULLUP_ENABLE,
//         .master.clk_speed = I2C_MASTER_FREQ_HZ,
//     };
//     i2c_param_config(I2C_MASTER_NUM, &conf);
//     i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
// }

// void app_main(void)
// {
//     i2c_master_init();

//     if (MPU6050_init(I2C_MASTER_NUM) == ESP_OK)
//     {
//         printf("MPU6050 initialized successfully!\n");
//     }
//     else
//     {
//         printf("MPU6050 initialization failed.\n");
//         return;
//     }

//     MPU6050_raw_data_t data;
//     MPU6050_data_t converted;

//     while (1)
//     {
//         if (MPU6050_read_raw(I2C_MASTER_NUM, &data) == ESP_OK)
//         {
//             MPU6050_convert(&data, &converted);
//             printf("Accel: X=%.2fg Y=%.2fg Z=%.2fg\n", converted.accel_x_g, converted.accel_y_g, converted.accel_z_g);
//             printf("Gyro:  X=%.2f°/s Y=%.2f°/s Z=%.2f°/s\n", converted.gyro_x_dps, converted.gyro_y_dps, converted.gyro_z_dps);
//         }
//         else
//         {
//             printf("Failed to read MPU6050 data\n");
//         }
//         vTaskDelay(pdMS_TO_TICKS(1000)); // 1-second delay
//     }
// }

#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_timer.h" // Include for accurate timing
#include "mpu6050.h"

#define I2C_MASTER_SCL_IO 36
#define I2C_MASTER_SDA_IO 37
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 400000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

#define ALPHA 0.98f // Complementary filter constant

// --- Gyro bias storage ---
static float gyro_x_bias = 0.0f;
static float gyro_y_bias = 0.0f;
static float gyro_z_bias = 0.0f;

static void i2c_master_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode,
                       I2C_MASTER_RX_BUF_DISABLE,
                       I2C_MASTER_TX_BUF_DISABLE, 0);
}

// --- Gyro calibration function ---
static void calibrate_gyro()
{
    MPU6050_raw_data_t raw;
    MPU6050_data_t converted;
    const int samples = 2000;
    float sum_x = 0, sum_y = 0, sum_z = 0;

    printf("Calibrating gyro... keep sensor still!\n");

    for (int i = 0; i < samples; i++)
    {
        if (MPU6050_read_raw(I2C_MASTER_NUM, &raw) == ESP_OK)
        {
            MPU6050_convert(&raw, &converted);
            sum_x += converted.gyro_x_dps;
            sum_y += converted.gyro_y_dps;
            sum_z += converted.gyro_z_dps;
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }

    gyro_x_bias = sum_x / samples;
    gyro_y_bias = sum_y / samples;
    gyro_z_bias = sum_z / samples;

    printf("Gyro calibration done: bias_x=%.3f, bias_y=%.3f, bias_z=%.3f\n",
           gyro_x_bias, gyro_y_bias, gyro_z_bias);
}

void app_main(void)
{
    i2c_master_init();

    if (MPU6050_init(I2C_MASTER_NUM) == ESP_OK)
    {
        printf("MPU6050 initialized successfully!\n");
    }
    else
    {
        printf("MPU6050 initialization failed.\n");
        return;
    }

    calibrate_gyro();

    MPU6050_raw_data_t data;
    MPU6050_data_t converted;

    float pitch = 0.0f, roll = 0.0f; // Filtered angles

    int64_t last_time = esp_timer_get_time(); // µs timestamp

    while (1)
    {
        int64_t now = esp_timer_get_time();
        float dt = (float)(now - last_time) / 1000000.0f; // seconds
        float gyro_pitch_rate;
        float gyro_roll_rate;
        float acc_pitch;
        float acc_roll;
        last_time = now;

        if (MPU6050_read_raw(I2C_MASTER_NUM, &data) == ESP_OK)
        {
            MPU6050_convert(&data, &converted);

            // --- Accelerometer angle estimation ---
            acc_pitch = atan2f(converted.accel_y_g,
                               sqrtf(converted.accel_x_g * converted.accel_x_g + converted.accel_z_g * converted.accel_z_g)) *
                        180.0f / M_PI;
            acc_roll = atan2f(-converted.accel_x_g, converted.accel_z_g) * 180.0f / M_PI;

            // --- Gyroscope integration (with bias removed) ---
            gyro_pitch_rate = converted.gyro_y_dps - gyro_y_bias;
            gyro_roll_rate = converted.gyro_x_dps - gyro_x_bias;

            // --- Complementary filter ---
            pitch = ALPHA * (pitch + gyro_pitch_rate * dt) + (1.0f - ALPHA) * acc_pitch;
            roll = ALPHA * (roll + gyro_roll_rate * dt) + (1.0f - ALPHA) * acc_roll;

            printf("Pitch: %.2f°, Roll: %.2f°, dt=%.3f s\n", pitch, roll, dt);
        }
        else
        {
            printf("Failed to read MPU6050 data\n");
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // loop ~100Hz, small delay to avoid maxing CPU
    }
}