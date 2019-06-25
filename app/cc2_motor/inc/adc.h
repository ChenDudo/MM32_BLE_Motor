// Define to prevent recursive inclusion  --------------------------------------
#ifndef __ADC_H
#define __ADC_H

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup EXAMPLE_MOTOR
/// @brief MOTOR example modules
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @defgroup MOTOR_Exported_Variables
/// @{
#ifdef _ADC_C_
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL u32   ADC1Value[2];

GLOBAL bool  adcVolFlag;
GLOBAL u16   adcVolTick;
GLOBAL float adcTemp;
GLOBAL float adcTmp;//

GLOBAL bool  adcTempFlag;
GLOBAL u16   adcTempTick;
GLOBAL u32   adcVolt;
GLOBAL u32   adcVol;//

GLOBAL u16  securMax;

#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MOTOR_Exported_Functions
/// @{

void adcTick();
void initADC();
void changeSecMax();

/// @}


/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
