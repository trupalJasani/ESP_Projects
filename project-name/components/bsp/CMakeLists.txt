idf_component_register(
    SRCS 
        "mpu6050/mpu6050_driver.c"
        "sht21/sht21_driver.c"
        "sensor_bus/sensor_bus.c"
    INCLUDE_DIRS 
        "mpu6050"
        "sht21"
        "sensor_bus"
    REQUIRES driver
)
