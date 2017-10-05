// Debug for accl
#define SERIAL_DEBUG_OUTPUT true

// Pixy
#define PIXY_WINDOW_LOW 106
#define PIXY_WINDOW_HIGH 213


// Ultrasonic
#define US_MIN_DIST 30  // In cm, found through testing
#define US_MAX_DIST 40 // "    "

#define DIST_HYST 10 // +-Value determined through testing because Pixy is less accurate than US
                     
// I2C
#define IMU_ADDRESS 0b1101000

// Gyro
#define IMU_GYRO_CONFIG 0x27
#define IMU_GYRO_X_H 0x43 // Higher byte
#define IMU_GYRO_X_L 0x44 // Lower byte

// Full Scale Range
#define IMU_GYRO_FSR_250 0x00
#define IMU_GYRO_FSR_500 0x08
#define IMU_GYRO_FSR_1000 0x10
#define IMU_GYRO_FSR_2000 0x18

// Sensitivity Scale Factor
#define IMU_GYRO_SSF_250 131
#define IMU_GYRO_SSF_500 65.5
#define IMU_GYRO_SSF_1000 32.8
#define IMU_GYRO_SSF_2000 16.4

// Accel
#define IMU_ACCEL_CONFIG 0x1C
#define IMU_ACCEL_X_H 0x3B // Higher byte
#define IMU_ACCEL_X_L 0X3C // Lower byte

// Full Scale Range
#define IMU_ACCEL_FSR_2 0x00
#define IMU_ACCEL_FSR_4 0x08
#define IMU_ACCEL_FSR_8 0x10
#define IMU_ACCEL_FSR_16 0x18

// Sensitivity Scale Factor
#define IMU_ACCEL_SSF_2 16384
#define IMU_ACCEL_SSF_4 8192
#define IMU_ACCEL_SSF_8 4096
#define IMU_ACCEL_SSF_16 2048

// Movement
#define WHEEL_DISTANCE 30 // Distance from center of each wheel in cm
#define WHEEL_RADIUS 5    // Radius of wheel in cm

// Conversion
#define PX_PER_METER 30

// Math
#define PI 3.14159