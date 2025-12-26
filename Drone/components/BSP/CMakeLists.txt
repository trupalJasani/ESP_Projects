idf_component_register(
    SRCS 
        "mpu6050/mpu6050.c"
        "sht21/sht21.c"
        "sensor_bus/sensor_bus.c"
        "sensor_fusion/sensor_fusion.c"
    INCLUDE_DIRS 
        "mpu6050"
        "sht21"
        "sensor_bus"
        "sensor_fusion"
    REQUIRES driver
)
