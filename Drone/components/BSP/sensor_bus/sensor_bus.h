#ifndef BSP_I2C_H
#define BSP_I2C_H

#include "esp_err.h"
#include "driver/i2c.h"

// --- I2C BUS Configuration (Used by all I2C BSP files) ---
#define I2C_MASTER_SDA_IO           37      
#define I2C_MASTER_SCL_IO           36    
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          400000  // 400kHz Fast Mode

// --- Sensor I2C Addresses ---
#define MPU6050_SENSOR_ADDR         0x68
#define SHT21_SENSOR_ADDR           0x40
/**
 * @brief Initializes the I2C master bus.
 * @return ESP_OK on success.
 */
esp_err_t bsp_i2c_init(void);

/**
 * @brief Generic I2C write function (sends register address + data).
 */
esp_err_t bsp_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);

/**
 * @brief Generic I2C read function (sends register address, then reads data).
 */
esp_err_t bsp_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);


#endif // BSP_I2C_H
