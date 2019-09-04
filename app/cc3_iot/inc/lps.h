// Define to prevent recursive inclusion  --------------------------------------
#ifndef __LPS_H
#define __LPS_H

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup EXAMPLE_LPS
/// @brief Pressure Sensor example modules
/// @{

typedef enum {
  LPS22HB_POWER_DOWN= 0,
  LPS22HB_ODR_1Hz   = 1,
  LPS22HB_ODR_10Hz  = 2,
  LPS22HB_ODR_25Hz  = 3,
  LPS22HB_ODR_50Hz  = 4,
  LPS22HB_ODR_75Hz  = 5,
} LPS22HB_ODR;

#define LPS22HB_ADDR        0xB8
#define LPS22HB_WHO_AM_I    0x0F
#define LPS22HB_ID          0xB1

#define LPS22HB_CTRL_REG1       0x10
#define LPS22HB_CTRL_REG2       0x11
#define LPS22HB_RES_CONF        0x1A
#define LPS22HB_PRESS_OUT_XL    0x28
#define LPS22HB_PRESS_OUT_L     0x29
#define LPS22HB_PRESS_OUT_H     0x2A

////////////////////////////////////////////////////////////////////////////////
/// @defgroup LPS_Exported_Variables
/// @{
#ifdef _LPS_C_
#define GLOBAL

#else
#define GLOBAL extern

#endif

GLOBAL bool lpsRight;

#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup LPS_Exported_Functions
/// @{

bool LPS22HB_Init();
void LPS22HB_Config();
double LPS22HB_Get_Press();

/// @}


/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
