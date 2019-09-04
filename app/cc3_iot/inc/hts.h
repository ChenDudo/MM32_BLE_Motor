// Define to prevent recursive inclusion  --------------------------------------
#ifndef __HTS_H
#define __HTS_H

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup EXAMPLE_HTS
/// @brief Humidity Temperature Sensor example modules
/// @{
#define HTS221_ADDR         0xBF
#define HTS221_WHO_AM_I     0x0F
#define HTS221_ID           0xBC

#define HTS221_AV_CONF          0x10
#define HTS221_CTRL_REG1        0x20
#define HTS221_HUMIDITY_OUT_L   0x28
#define HTS221_HUMIDITY_OUT_H   0x29
#define HTS221_TEMP_OUT_L       0x2A
#define HTS221_TEMP_OUT_H       0x2B

////////////////////////////////////////////////////////////////////////////////
/// @defgroup HTS_Exported_Variables
/// @{
#ifdef _HTS_C_
#define GLOBAL

#else
#define GLOBAL extern

#endif

GLOBAL bool htsRight;
GLOBAL float k_Temp;
GLOBAL float b_Temp;
GLOBAL float k_Hum;
GLOBAL float b_Hum;

#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup HTS_Exported_Functions
/// @{

bool HTS221_Init();
void HTS221_Get_Equation_Temp();
float HTS221_Get_AVTemp();
void HTS221_Get_Equation_Hum();
float HTS221_Get_AVHum();

/// @}


/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
