// mpu6050_reg.h

#ifndef MPU6050_REGS_H
#define MPU6050_REGS_H

// MPU6050 I2C Address
#define MPU6050_I2C_ADDR     0x69  // Ad0 line is high so that address is 0x69 otherwise address is 0x68

// WHO_AM_I expected value
#define MPU6050_WHO_AM_I_ID  0x70

// Register Map
typedef enum {
    MPU6050_SELF_TEST_X_GYRO    = 0x00,
    MPU6050_SELF_TEST_Y_GYRO    = 0x01,
    MPU6050_SELF_TEST_Z_GYRO    = 0x02,
    MPU6050_SELF_TEST_X_ACCEL   = 0x0D,
    MPU6050_SELF_TEST_Y_ACCEL   = 0x0E,
    MPU6050_SELF_TEST_Z_ACCEL   = 0x0F,

    MPU6050_XG_OFFSET_H         = 0x13,
    MPU6050_XG_OFFSET_L         = 0x14,
    MPU6050_YG_OFFSET_H         = 0x15,
    MPU6050_YG_OFFSET_L         = 0x16,
    MPU6050_ZG_OFFSET_H         = 0x17,
    MPU6050_ZG_OFFSET_L         = 0x18,

    MPU6050_SMPLRT_DIV          = 0x19,
    MPU6050_CONFIG              = 0x1A,
    MPU6050_GYRO_CONFIG         = 0x1B,
    MPU6050_ACCEL_CONFIG        = 0x1C,
    MPU6050_ACCEL_CONFIG2       = 0x1D,
    MPU6050_LP_MODE_CFG         = 0x1E,
    MPU6050_ACCEL_WOM_X_THR     = 0x20,
    MPU6050_ACCEL_WOM_Y_THR     = 0x21,
    MPU6050_ACCEL_WOM_Z_THR     = 0x22,

    MPU6050_FIFO_EN             = 0x23,
    MPU6050_INT_PIN_CFG         = 0x37,
    MPU6050_INT_ENABLE          = 0x38,
    MPU6050_INT_STATUS          = 0x3A,

    MPU6050_ACCEL_XOUT_H        = 0x3B,
    MPU6050_ACCEL_XOUT_L        = 0x3C,
    MPU6050_ACCEL_YOUT_H        = 0x3D,
    MPU6050_ACCEL_YOUT_L        = 0x3E,
    MPU6050_ACCEL_ZOUT_H        = 0x3F,
    MPU6050_ACCEL_ZOUT_L        = 0x40,

    MPU6050_TEMP_OUT_H          = 0x41,
    MPU6050_TEMP_OUT_L          = 0x42,

    MPU6050_GYRO_XOUT_H         = 0x43,
    MPU6050_GYRO_XOUT_L         = 0x44,
    MPU6050_GYRO_YOUT_H         = 0x45,
    MPU6050_GYRO_YOUT_L         = 0x46,
    MPU6050_GYRO_ZOUT_H         = 0x47,
    MPU6050_GYRO_ZOUT_L         = 0x48,

    MPU6050_SIGNAL_PATH_RESET   = 0x68,
    MPU6050_ACCEL_INTEL_CTRL    = 0x69,
    MPU6050_USER_CTRL           = 0x6A,
    MPU6050_PWR_MGMT_1          = 0x6B,
    MPU6050_PWR_MGMT_2          = 0x6C,
    MPU6050_WHO_AM_I            = 0x75
} MPU6050_reg_t;

#endif // MPU6050_REGS_H