// Define to prevent recursive inclusion  --------------------------------------
#ifndef __LSM_H
#define __LSM_H

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup EXAMPLE_LSM
/// @brief Pressure Sensor example modules
/// @{
typedef enum {
  LSM6DSL_2g       = 0,
  LSM6DSL_16g      = 1,
  LSM6DSL_4g       = 2,
  LSM6DSL_8g       = 3,
  LSM6DSL_XL_FS_ND = 4,
} LSM6DSL_FS_XL;

typedef enum {
  LSM6DSL_250dps     = 0,
  LSM6DSL_125dps     = 1,
  LSM6DSL_500dps     = 2,
  LSM6DSL_1000dps    = 4,
  LSM6DSL_2000dps    = 6,
  LSM6DSL_GY_FS_ND   = 7,
} LSM6DSL_FS_G;

typedef enum {
  LSM6DSL_XL_ODR_OFF      =  0,
  LSM6DSL_XL_ODR_12Hz5    =  1,
  LSM6DSL_XL_ODR_26Hz     =  2,
  LSM6DSL_XL_ODR_52Hz     =  3,
  LSM6DSL_XL_ODR_104Hz    =  4,
  LSM6DSL_XL_ODR_208Hz    =  5,
  LSM6DSL_XL_ODR_416Hz    =  6,
  LSM6DSL_XL_ODR_833Hz    =  7,
  LSM6DSL_XL_ODR_1k66Hz   =  8,
  LSM6DSL_XL_ODR_3k33Hz   =  9,
  LSM6DSL_XL_ODR_6k66Hz   = 10,
  LSM6DSL_XL_ODR_1Hz6     = 11,
  LSM6DSL_XL_ODR_ND       = 12,
} LSM6DSL_ODR_XL;

typedef enum {
  LSM6DSL_DEG_80      = 0,
  LSM6DSL_DEG_70      = 1,
  LSM6DSL_DEG_60      = 2,
  LSM6DSL_DEG_50      = 3,
  LSM6DSL_6D_TH_ND    = 4,
} LSM6DSL_SIXD_THS;

typedef enum
{
  LSM6DSL_INT1_PIN,
  LSM6DSL_INT2_PIN,
} LSM6DSL_INTPIN;

typedef struct
{
  s32 x;
  s32 y;
  s32 z;
} LSM6DSL_AXES;

#define LSM6DSL_ADDR        0xD4
#define LSM6DSL_WHO_AM_I    0x0F
#define LSM6DSL_ID          0x6A

#define LSM6DSL_FIFO_CTRL5          0x0A
#define LSM6DSL_DRDY_PULSE_CFG_G    0x0B
#define LSM6DSL_INT1_CTRL           0x0D
#define LSM6DSL_INT2_CTRL           0x0E
#define LSM6DSL_CTRL1_XL            0x10
#define LSM6DSL_CTRL2_G             0x11
#define LSM6DSL_CTRL3_C             0x12
#define LSM6DSL_CTRL4_C             0x13
#define LSM6DSL_CTRL8_XL            0x17
#define LSM6DSL_CTRL10_C            0x19
#define LSM6DSL_MASTER_CONFIG       0x1A
#define LSM6DSL_WAKE_UP_SRC         0x1B
#define LSM6DSL_TAP_SRC             0x1C
#define LSM6DSL_D6D_SRC             0x1D
#define LSM6DSL_OUT_TEMP_L          0x20
#define LSM6DSL_OUT_TEMP_H          0x21
#define LSM6DSL_OUTX_L_G            0x22
#define LSM6DSL_OUTX_H_G            0x23
#define LSM6DSL_OUTY_L_G            0x24
#define LSM6DSL_OUTY_H_G            0x25
#define LSM6DSL_OUTZ_L_G            0x26
#define LSM6DSL_OUTZ_H_G            0x27
#define LSM6DSL_OUTX_L_XL           0x28
#define LSM6DSL_OUTX_H_XL           0x29
#define LSM6DSL_OUTY_L_XL           0x2A
#define LSM6DSL_OUTY_H_XL           0x2B
#define LSM6DSL_OUTZ_L_XL           0x2C
#define LSM6DSL_OUTZ_H_XL           0x2D
#define LSM6DSL_SENSORHUB1_REG      0x2E
#define LSM6DSL_FUNC_SRC1           0x53
#define LSM6DSL_TAP_CFG             0x58
#define LSM6DSL_TAP_THS_6D          0x59
#define LSM6DSL_WAKE_UP_THS         0x5B
#define LSM6DSL_WAKE_UP_DUR         0x5C
#define LSM6DSL_MD1_CFG             0x5E
#define LSM6DSL_MD2_CFG             0x5F

#define LSM6DSL_ACC_SENSITIVITY_FS_2G   0.061f
#define LSM6DSL_ACC_SENSITIVITY_FS_4G   0.122f
#define LSM6DSL_ACC_SENSITIVITY_FS_8G   0.244f
#define LSM6DSL_ACC_SENSITIVITY_FS_16G  0.488f

#define LSM6DSL_GYRO_SENSITIVITY_FS_125DPS    4.375f
#define LSM6DSL_GYRO_SENSITIVITY_FS_250DPS    8.750f
#define LSM6DSL_GYRO_SENSITIVITY_FS_500DPS   17.500f
#define LSM6DSL_GYRO_SENSITIVITY_FS_1000DPS  35.000f
#define LSM6DSL_GYRO_SENSITIVITY_FS_2000DPS  70.000f

////////////////////////////////////////////////////////////////////////////////
/// @defgroup LSM_Exported_Variables
/// @{
#ifdef _LSM_C_
#define GLOBAL

#else
#define GLOBAL extern

#endif

GLOBAL bool lsmRight;
GLOBAL u8 int1Route[4];
GLOBAL u8 int2Route[3];

#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup LSM_Exported_Functions
/// @{

bool LSM6DSL_Init();
void LSM6DSL_6DOrientation_Init();
void LSM6DSL_6DOrientation_Process();
void Get_Angular_Rate(LSM6DSL_AXES* angularRate);
void Get_Acceleration_Rate(LSM6DSL_AXES* accRate);
/// @}


/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
