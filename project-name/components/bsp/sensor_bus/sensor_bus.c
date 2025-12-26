#include "sensor_bus.h"

esp_err_t sensor_bus_init(i2c_port_t i2c_num, uint8_t sda_pin, uint8_t scl_pin)
{

    i2c_port = i2c_num;

    i2c_config_t bus_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = SPEED_STANDARD,
    };

    esp_err_t ret = i2c_param_config(i2c_port, &bus_config);
    if (ret != ESP_OK)
        return ret;
    printf("Param setting is done\r\n");

    ret = i2c_driver_install(i2c_port, bus_config.mode,
                             0,
                             0, 0);
    if (ret != ESP_OK)
        return ret;
    printf("sht21 driver install successfully\r\n");

    return ESP_OK;
}