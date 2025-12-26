// mpu6500_regs.h

#ifndef MPU6500_REGS_H
#define MPU6500_REGS_H

// MPU6500 I2C Address
#define MPU6500_I2C_ADDR     0x69  // Ad0 line is high so that address is 0x69 otherwise address is 0x68

// WHO_AM_I expected value
#define MPU6500_WHO_AM_I_ID  0x70

// Register Map
typedef enum {
    MPU6500_SELF_TEST_X_GYRO    = 0x00,
    MPU6500_SELF_TEST_Y_GYRO    = 0x01,
    MPU6500_SELF_TEST_Z_GYRO    = 0x02,
    MPU6500_SELF_TEST_X_ACCEL   = 0x0D,
    MPU6500_SELF_TEST_Y_ACCEL   = 0x0E,
    MPU6500_SELF_TEST_Z_ACCEL   = 0x0F,

    MPU6500_XG_OFFSET_H         = 0x13,
    MPU6500_XG_OFFSET_L         = 0x14,
    MPU6500_YG_OFFSET_H         = 0x15,
    MPU6500_YG_OFFSET_L         = 0x16,
    MPU6500_ZG_OFFSET_H         = 0x17,
    MPU6500_ZG_OFFSET_L         = 0x18,

    MPU6500_SMPLRT_DIV          = 0x19,
    MPU6500_CONFIG              = 0x1A,
    MPU6500_GYRO_CONFIG         = 0x1B,
    MPU6500_ACCEL_CONFIG        = 0x1C,
    MPU6500_ACCEL_CONFIG2       = 0x1D,
    MPU6500_LP_MODE_CFG         = 0x1E,
    MPU6500_ACCEL_WOM_X_THR     = 0x20,
    MPU6500_ACCEL_WOM_Y_THR     = 0x21,
    MPU6500_ACCEL_WOM_Z_THR     = 0x22,

    MPU6500_FIFO_EN             = 0x23,
    MPU6500_INT_PIN_CFG         = 0x37,
    MPU6500_INT_ENABLE          = 0x38,
    MPU6500_INT_STATUS          = 0x3A,

    MPU6500_ACCEL_XOUT_H        = 0x3B,
    MPU6500_ACCEL_XOUT_L        = 0x3C,
    MPU6500_ACCEL_YOUT_H        = 0x3D,
    MPU6500_ACCEL_YOUT_L        = 0x3E,
    MPU6500_ACCEL_ZOUT_H        = 0x3F,
    MPU6500_ACCEL_ZOUT_L        = 0x40,

    MPU6500_TEMP_OUT_H          = 0x41,
    MPU6500_TEMP_OUT_L          = 0x42,

    MPU6500_GYRO_XOUT_H         = 0x43,
    MPU6500_GYRO_XOUT_L         = 0x44,
    MPU6500_GYRO_YOUT_H         = 0x45,
    MPU6500_GYRO_YOUT_L         = 0x46,
    MPU6500_GYRO_ZOUT_H         = 0x47,
    MPU6500_GYRO_ZOUT_L         = 0x48,

    MPU6500_SIGNAL_PATH_RESET   = 0x68,
    MPU6500_ACCEL_INTEL_CTRL    = 0x69,
    MPU6500_USER_CTRL           = 0x6A,
    MPU6500_PWR_MGMT_1          = 0x6B,
    MPU6500_PWR_MGMT_2          = 0x6C,
    MPU6500_WHO_AM_I            = 0x75
} mpu6500_reg_t;

#endif // MPU6500_REGS_H
