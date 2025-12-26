#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "mpu6500.h"

#define I2C_MASTER_SCL_IO 36
#define I2C_MASTER_SDA_IO 37
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 400000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

#define ACCEL_THRESHOLD 0.3f // for tilt direction
#define GYRO_THRESHOLD 30.0f // for rotation detection

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
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void app_main(void)
{
    i2c_master_init();

    if (mpu6500_init(I2C_MASTER_NUM) == ESP_OK)
    {
        printf("MPU6500 initialized successfully!\n");
    }
    else
    {
        printf("MPU6500 initialization failed.\n");
        return;
    }

    mpu6500_raw_data_t data;
    mpu6500_data_t converted;

    while (1)
    {
        if (mpu6500_read_raw(I2C_MASTER_NUM, &data) == ESP_OK)
        {
            mpu6500_convert(&data, &converted);

            printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
                   converted.accel_x_g, converted.accel_y_g, converted.accel_z_g,
                   converted.gyro_x_dps, converted.gyro_y_dps, converted.gyro_z_dps);
            printf("Accel: X=%.2fg Y=%.2fg Z=%.2fg\n", converted.accel_x_g, converted.accel_y_g, converted.accel_z_g);
            printf("Gyro:  X=%.2f°/s Y=%.2f°/s Z=%.2f°/s\n", converted.gyro_x_dps, converted.gyro_y_dps, converted.gyro_z_dps);

                if (converted.accel_x_g > ACCEL_THRESHOLD)
                {
                    printf("Tilted RIGHT (Accel +X)\n");
                }
                else if (converted.accel_x_g < -ACCEL_THRESHOLD)
                {
                    printf("Tilted LEFT (Accel -X)\n");
                }
                else
                {
                    printf("Accel: LEVEL on X\n");
                }

                if (converted.accel_y_g > ACCEL_THRESHOLD)
                {
                    printf("Tilted UP (Accel +Y)\n");
                }
                else if (converted.accel_y_g < -ACCEL_THRESHOLD)
                {
                    printf("Tilted DOWN (Accel -Y)\n");
                }
                else
                {
                    printf("Accel: LEVEL on Y\n");
                }

                // Gyroscope rotation direction
                if (converted.gyro_x_dps > GYRO_THRESHOLD)
                {
                    printf("Rotating FORWARD (Gyro +X)\n");
                }
                else if (converted.gyro_x_dps < -GYRO_THRESHOLD)
                {
                    printf("Rotating BACKWARD (Gyro -X)\n");
                }

                if (converted.gyro_y_dps > GYRO_THRESHOLD)
                {
                    printf("Rolling RIGHT (Gyro +Y)\n");
                }
                else if (converted.gyro_y_dps < -GYRO_THRESHOLD)
                {
                    printf("Rolling LEFT (Gyro -Y)\n");
                }

                if (converted.gyro_z_dps > GYRO_THRESHOLD)
                {
                    printf("Yawing CLOCKWISE (Gyro +Z)\n");
                }
                else if (converted.gyro_z_dps < -GYRO_THRESHOLD)
                {
                    printf("Yawing COUNTERCLOCKWISE (Gyro -Z)\n");
                }

                printf("\n");
        }
        else
        {
            printf("Failed to read MPU6500 data\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1-second delay
    }
}