#include "sensor_fusion.h"
#include "sensor_bus.h"
#include "mpu6050.h"
#include "sht21.h"
#include <math.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *ABS_TAG = "ABSTRACT_LAYER";

FlightData_t flight_data = {0};

// MPU6050 constants (for a typical +/- 2g and +/- 250 deg/s configuration)
#define MPU_ACCEL_SENSITIVITY 16384.0f
#define MPU_GYRO_SENSITIVITY 131.0f

// SHT21 conversion constants
#define SHT21_DIVISOR 65536.0f
#define SHT21_OFFSET -46.85f
#define SHT21_SLOPE 175.72f

// Sensor Fusion State (Complementary Filter)
float current_roll = 0.0f;
float current_pitch = 0.0f;
const float COMPLEMENTARY_FILTER_ALPHA = 0.98f; // Gyro weight
const float LOOP_TIME_S = 0.01f; // Assumes 100Hz loop

/**
 * @brief Processes MPU raw data, applies conversions, and runs the Complementary Filter.
 */
static void abstract_mpu_process(int16_t accel_raw[3], int16_t gyro_raw[3]) {
    // 1. Convert Raw to Physical Units (Gyro Rate in deg/s)
    float Gx = (float)gyro_raw[0] / MPU_GYRO_SENSITIVITY;
    float Gy = (float)gyro_raw[1] / MPU_GYRO_SENSITIVITY;
    
    // 2. Convert Raw to Physical Units (Accel in G's)
    float Ax = (float)accel_raw[0] / MPU_ACCEL_SENSITIVITY;
    float Ay = (float)accel_raw[1] / MPU_ACCEL_SENSITIVITY;
    float Az = (float)accel_raw[2] / MPU_ACCEL_SENSITIVITY;

    // 3. Accelerometer-derived angles (slow reference)
    float accel_roll = atan2f(Ay, Az) * (180.0f / M_PI);
    float accel_pitch = atan2f(-Ax, sqrtf(Ay*Ay + Az*Az)) * (180.0f / M_PI);

    // 4. Gyro integration (fast update)
    current_roll += Gx * LOOP_TIME_S;
    current_pitch += Gy * LOOP_TIME_S;
    
    // 5. Complementary Filter (Fusion)
    // Fused Angle = (Gyro Angle * Alpha) + (Accel Angle * (1 - Alpha))
    current_roll = COMPLEMENTARY_FILTER_ALPHA * current_roll + (1.0f - COMPLEMENTARY_FILTER_ALPHA) * accel_roll;
    current_pitch = COMPLEMENTARY_FILTER_ALPHA * current_pitch + (1.0f - COMPLEMENTARY_FILTER_ALPHA) * accel_pitch;

    // 6. Update global struct
    flight_data.roll = current_roll;
    flight_data.pitch = current_pitch;
    flight_data.yaw_rate = (float)gyro_raw[2] / MPU_GYRO_SENSITIVITY;
}

/**
 * @brief Converts SHT21 raw code to physical temperature (Celsius).
 */
static float abstract_sht21_process(uint16_t temp_raw) {
    return SHT21_OFFSET + SHT21_SLOPE * ((float)temp_raw / SHT21_DIVISOR);
}


esp_err_t abstract_init(void) {
    ESP_LOGI(ABS_TAG, "Initializing BSP I2C...");
    if (bsp_i2c_init() != ESP_OK) return ESP_FAIL;

    ESP_LOGI(ABS_TAG, "Initializing MPU6050...");
    if (bsp_mpu6050_init() != ESP_OK) return ESP_FAIL;

    // Reset sensor fusion state
    current_roll = 0.0f;
    current_pitch = 0.0f;

    // TODO: Initialize LEDC/PWM for Motor Control here (frequency, timer, channels)

    return ESP_OK;
}

esp_err_t abstract_read_and_process(void) {
    int16_t accel_raw[3];
    int16_t gyro_raw[3];
    uint16_t temp_raw;

    // 1. Read Raw Data (BSP calls)
    if (bsp_mpu6050_read_raw(accel_raw, gyro_raw) != ESP_OK) return ESP_FAIL;
    if (bsp_sht21_read_raw(&temp_raw) != ESP_OK) return ESP_FAIL;

    // 2. Process MPU data and apply sensor fusion
    abstract_mpu_process(accel_raw, gyro_raw);

    // 3. Process SHT21 data
    flight_data.temperature = abstract_sht21_process(temp_raw);

    // 4. Update Altitude (Placeholder)
    flight_data.altitude = 0.0f; // TODO: Implement Barometer/Sonar reading here

    return ESP_OK;
}

void app_motor_control(float roll_cmd, float pitch_cmd, float yaw_cmd, float throttle) {
    // This is the motor mixer, converting desired rotation commands into motor outputs.
    // The inputs (cmd, throttle) should be between 0.0 and 1.0.

    // Calculate Motor Mix (Standard Quadcopter X-configuration)
    float m1 = throttle - pitch_cmd - roll_cmd - yaw_cmd; // Front Right
    float m2 = throttle + pitch_cmd - roll_cmd + yaw_cmd; // Back Right
    float m3 = throttle + pitch_cmd + roll_cmd - yaw_cmd; // Back Left
    float m4 = throttle - pitch_cmd + roll_cmd + yaw_cmd; // Front Left

    // Clamp normalized values
    m1 = fmaxf(0.0f, fminf(1.0f, m1));
    m2 = fmaxf(0.0f, fminf(1.0f, m2));
    m3 = fmaxf(0.0f, fminf(1.0f, m3));
    m4 = fmaxf(0.0f, fminf(1.0f, m4));

    // TODO: Convert m1-m4 (0.0-1.0) into actual ESC PWM duty cycles (e.g., 1000us to 2000us)
}
