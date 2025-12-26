#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sensor_fusion.h" // Includes FlightData_t and abstract functions

// --- APPLICATION LAYER DEFINITIONS (State Machine) ---

static const char *TAG = "DRONE_APP";

// Define the operational states
typedef enum {
    IDLE,
    CALIBRATING, // Running sensor calibration and initial checks
    ARMED,       // Ready to take off (Motors armed, waiting for throttle)
    FLYING,
    LANDING,
    ERROR
} DroneState_t;

DroneState_t current_state = IDLE;

/**
 * @brief Main drone state machine loop running in a FreeRTOS task.
 */
void state_machine_loop(void *pvParameters) {
    const TickType_t xDelay = pdMS_TO_TICKS(10); // 10ms loop time (100Hz flight loop rate)
    
    // Safety check on hardware initialization (from Abstract Layer)
    if (abstract_init() != ESP_OK) {
        ESP_LOGE(TAG, "Hardware initialization FAILED. Entering permanent ERROR state.");
        current_state = ERROR;
        vTaskDelete(NULL);
    }
    current_state = IDLE;

    while (1) {
        switch (current_state) {
            case IDLE:
                ESP_LOGI(TAG, "System IDLE. Ready for ARM command.");
                current_state = CALIBRATING; // Auto-transition for demo
                break;

            case CALIBRATING: {
                ESP_LOGW(TAG, "CALIBRATING... Do not move drone.");
                // Read and process sensors for a short duration to establish zero biases
                int calibration_steps = 100;
                for (int i = 0; i < calibration_steps; i++) {
                    abstract_read_and_process();
                    // TODO: Accumulate and average gyro/accel biases here.
                    vTaskDelay(pdMS_TO_TICKS(5));
                }
                ESP_LOGI(TAG, "Calibration complete. Roll: %.2f, Pitch: %.2f", flight_data.roll, flight_data.pitch);
                current_state = ARMED;
                break;
            }

            case ARMED:
                // Fast loop for monitoring and idle motor spinning
                abstract_read_and_process();
                ESP_LOGD(TAG, "ARMED. Temp: %.1f C | Roll: %.1f deg", flight_data.temperature, flight_data.roll);

                // Keep motors spinning at minimum idle speed
                app_motor_control(0, 0, 0, 0.1f); 
                
                // TODO: Check RC input: If throttle > idle, transition to FLYING
                // Example: if (rc_input.throttle > 0.15) current_state = FLYING;
                break;

            case FLYING:
                // 1. Read and process sensor data
                if (abstract_read_and_process() != ESP_OK) {
                    current_state = ERROR;
                    break;
                }

                // 2. Compute Control Commands (PID Controllers)
                // TODO: Replace these with actual PID calculations!
                float desired_roll = 0.0f; // From RC receiver
                float desired_pitch = 0.0f; // From RC receiver
                float desired_yaw_rate = 0.0f; // From RC receiver
                float current_throttle = 0.5f; // From RC receiver (mid-stick)

                // Dummy P-Control for demonstration: Error * Gain
                float roll_command = (desired_roll - flight_data.roll) * 0.1f; 
                float pitch_command = (desired_pitch - flight_data.pitch) * 0.1f;
                float yaw_command = (desired_yaw_rate - flight_data.yaw_rate) * 0.05f; 

                // 3. Apply outputs to motors
                app_motor_control(roll_command, pitch_command, yaw_command, current_throttle);
                ESP_LOGI(TAG, "FLYING! Roll: %.1f, Pitch: %.1f, Yaw Rate: %.1f", 
                    flight_data.roll, flight_data.pitch, flight_data.yaw_rate);
                    
                // TODO: Implement transition logic to LANDING (e.g., low throttle detected)
                break;

            case LANDING:
                // Implement soft landing sequence
                app_motor_control(0, 0, 0, 0.0f); // Emergency cut
                current_state = IDLE;
                break;

            case ERROR:
                // Stop all motors
                app_motor_control(0, 0, 0, 0.0f);
                ESP_LOGE(TAG, "CRITICAL ERROR STATE. Motors off.");
                vTaskDelay(pdMS_TO_TICKS(1000));
                break;
        }

        vTaskDelay(xDelay);
    }
}

/**
 * @brief Application entry point.
 */
void app_main(void) {
    // Start the main state machine task
    xTaskCreate(state_machine_loop, "state_machine", 4096, NULL, 5, NULL);
}
