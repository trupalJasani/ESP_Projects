#ifndef ABSTRACT_SENSORS_H
#define ABSTRACT_SENSORS_H

#include "esp_err.h"

// Structure to hold processed flight data (Abstraction Layer Output)
typedef struct {
    float roll;     // Degrees (Filtered/Fused)
    float pitch;    // Degrees (Filtered/Fused)
    float yaw_rate; // Degrees/second (Raw or filtered)
    float altitude; // Placeholder for baro/sonar sensor
    float temperature; // SHT21 Temperature (for logging/thermal monitoring)
} FlightData_t;

// Global structure instance accessible by the Application layer
extern FlightData_t flight_data;

/**
 * @brief Initializes all hardware resources (BSP) and sensor fusion settings.
 * @return ESP_OK on success.
 */
esp_err_t abstract_init(void);

/**
 * @brief Reads all sensors from BSP, processes the data using fusion/conversion, and updates flight_data.
 * @return ESP_OK on success.
 */
esp_err_t abstract_read_and_process(void);

/**
 * @brief Placeholder for Motor Control/PWM driver logic using normalized commands (0.0 to 1.0).
 */
void app_motor_control(float roll_cmd, float pitch_cmd, float yaw_cmd, float throttle);

#endif // ABSTRACT_SENSORS_H
