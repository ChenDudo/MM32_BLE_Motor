// Define to prevent recursive inclusion  --------------------------------------
#ifndef __MOTOR_H
#define __MOTOR_H

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup EXAMPLE_MOTOR
/// @brief MOTOR example modules
/// @{

#define PWM_DEFAULT_NUM 50
////////////////////////////////////////////////////////////////////////////////
/// @defgroup MOTOR_Exported_Variables
/// @{
#ifdef _MOTOR_C_
#define GLOBAL
#else
#define GLOBAL extern
#endif

//
GLOBAL u16 gDelaynum;
GLOBAL u8  pwmListCnt;
GLOBAL u16 gDelayTime;
///////////////////////
GLOBAL bool breathFlag;
GLOBAL u16  breathValue;

GLOBAL u16  pwmSetFlag;
GLOBAL u16  pwmSetValue;

GLOBAL u16  finalPWMValueMax;
GLOBAL u16  finalBreathLen;

#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MOTOR_Exported_Functions
/// @{

void motorTick();
void initDCmotor(void);
void writeBreathLen();

void initTimer16();
void OnTimer16();
void OffTimer16();

/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
