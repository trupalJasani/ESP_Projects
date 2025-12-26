#ifndef BSP_SHT21_H
#define BSP_SHT21_H

#include "esp_err.h"
#include <stdint.h>

/**
 * @brief Reads raw temperature data from SHT21.
 * @param temp_raw Output raw 16-bit temperature code.
 * @return ESP_OK on success.
 */
esp_err_t bsp_sht21_read_raw(uint16_t *temp_raw);

#endif // BSP_SHT21_H
