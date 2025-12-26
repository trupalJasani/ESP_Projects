#include "driver/i2c.h"
#include "driver/gpio.h"
#define SPEED_STANDARD 400000


i2c_port_t i2c_port;

esp_err_t sensor_bus_init(i2c_port_t i2c_num , uint8_t sda_pin , uint8_t scl_pin);
