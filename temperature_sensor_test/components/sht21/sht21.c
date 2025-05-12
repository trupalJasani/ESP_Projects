#include "sht21.h"

i2c_port_t i2c_port;

esp_err_t sht21_init(i2c_port_t i2c_num)
{
    i2c_port = i2c_num;

    i2c_config_t sht21_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SHT21_SDA_PIN,
        .scl_io_num = SHT21_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = SHT21_SPEED_STANDARD,
    };

    esp_err_t ret = i2c_param_config(i2c_port, &sht21_config);
    if (ret != ESP_OK) return ret;
    printf("Param setting is done\r\n");

    ret = i2c_driver_install(i2c_port, sht21_config.mode,
                             I2C_MASTER_RX_BUF_DISABLE,
                             I2C_MASTER_TX_BUF_DISABLE, 0);
    if (ret != ESP_OK) return ret;
    printf("sht21 driver install successfully\r\n");

    return ESP_OK;
}

TickType_t get_delay_for_measurement(sht21_command_t command)
{
    switch (command)
    {
        case SHT21_CMD_TRIG_T_MEASUREMENT_HM:
        case SHT21_CMD_TRIG_T_MEASUREMENT_NHM:
            return SHT21_DELAY_T_MEASUREMENT;

        case SHT21_CMD_TRIG_RH_MEASUREMENT_HM:
        case SHT21_CMD_TRIG_RH_MEASUREMENT_NHM:
            return SHT21_DELAY_RH_MEASUREMENT;

        default:
            assert(false); // Invalid command
            return 0;
    }
}

esp_err_t crc_checksum(uint8_t data_arr[], uint8_t data_len, uint8_t checksum)
{
    uint8_t crc = 0;

    for (uint8_t i = 0; i < data_len; ++i)
    {
        crc ^= data_arr[i];
        for (uint8_t b = 8; b > 0; --b)
        {
            crc = (crc & 0x80) ? (crc << 1) ^ CRC_POLYNOMIAL : (crc << 1);
        }
    }

    if (crc != checksum)
    {
        printf("CRC Error: Expected %02X but got %02X\n", checksum, crc);
        return ESP_ERR_INVALID_CRC;
    }

    return ESP_OK;
}

esp_err_t read_register(uint16_t *raw_data, sht21_command_t command)
{
    esp_err_t err;
    uint8_t data[2], checksum;

    // Send measurement command
    i2c_cmd_handle_t write_cmd = i2c_cmd_link_create();
    ER(i2c_master_start(write_cmd));
    ER(i2c_master_write_byte(write_cmd, (SHT21_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK));
    ER(i2c_master_write_byte(write_cmd, command, I2C_MASTER_ACK));
    ER(i2c_master_stop(write_cmd));
    err = i2c_master_cmd_begin(i2c_port, write_cmd, I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(write_cmd);
    ER(err);

    // Wait for measurement to complete
    vTaskDelay(get_delay_for_measurement(command));

    // Read data and checksum
    i2c_cmd_handle_t read_cmd = i2c_cmd_link_create();
    ER(i2c_master_start(read_cmd));
    ER(i2c_master_write_byte(read_cmd, (SHT21_I2C_ADDRESS << 1) | I2C_MASTER_READ, I2C_MASTER_ACK));
    ER(i2c_master_read(read_cmd, data, sizeof(data), I2C_MASTER_ACK));
    ER(i2c_master_read_byte(read_cmd, &checksum, I2C_MASTER_NACK));
    ER(i2c_master_stop(read_cmd));
    err = i2c_master_cmd_begin(i2c_port, read_cmd, I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(read_cmd);  
    ER(err);

    // Validate CRC
    err = crc_checksum(data, sizeof(data), checksum);
    if (err != ESP_OK) return err;

    // Convert data
    *raw_data = (data[0] << 8) | (data[1] & 0xFC);
    return ESP_OK;
}

esp_err_t sht21_get_temperature(float *dst)
{
    uint16_t raw_reading;
    ER(read_register(&raw_reading, SHT21_CMD_TRIG_T_MEASUREMENT_NHM));
    *dst = -46.85 + 175.72 * (float)raw_reading / 65536.0;
    return ESP_OK;
}

esp_err_t sht21_get_humidity(float *dst)
{
    uint16_t raw_reading;
    ER(read_register(&raw_reading, SHT21_CMD_TRIG_RH_MEASUREMENT_NHM));
    *dst = -6.0 + 125.0 * (float)raw_reading / 65536.0;
    return ESP_OK;
}
