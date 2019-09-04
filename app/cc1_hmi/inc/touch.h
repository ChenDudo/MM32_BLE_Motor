// Define to prevent recursive inclusion  --------------------------------------
#ifndef __TOUCH_H
#define __TOUCH_H

#include "queue.h"

#define TPAD_ARR_MAX_VAL    0XFFFF
//#define TPAD_GATE_VAL		100  

////////////////////////////////////////////////////////////////////////////////
/// @defgroup HMI_Exported_Variables
/// @{
#ifdef _TOUCH_C_
#define GLOBAL

u8 keyen2 = 0;
u8 keyen3 = 0;
u8 keyen4 = 0;


u16 tpad_default_val2 = 0; 
u16 tpad_default_val3 = 0; 
u16 tpad_default_val4 = 0; 

u16 startJudge_GATE = 	180;
u16 TPAD_GATE_VAL	=	200;
u16 TPAD_GATE_VAL2	=	1500;

bool gate_bigFlag		= false;
bool gate_smallFlag 	= false;
bool adjustDefaultFlag	= false;

u16 starttime = 0;

#else
#define GLOBAL extern

#endif

GLOBAL u8 keyen2;
GLOBAL u8 keyen3;
GLOBAL u8 keyen4;


GLOBAL u16 tpad_default_val2;                                                   
GLOBAL u16 tpad_default_val3;                                                   
GLOBAL u16 tpad_default_val4;                                                   

GLOBAL u16 startJudge_GATE;
GLOBAL u16 TPAD_GATE_VAL;
GLOBAL u16 TPAD_GATE_VAL2;

GLOBAL bool gate_bigFlag;
GLOBAL bool gate_smallFlag;
GLOBAL bool adjustDefaultFlag;

GLOBAL u16 starttime;

#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup HMI_Exported_Functions
/// @{



u8 TPAD_Init(u8 psc);
void TPAD_Reset2(void);
void TPAD_Reset3(void);
void TPAD_Reset4(void);
u16 TPAD_Get_Val2(void);
u16 TPAD_Get_Val3(void);
u16 TPAD_Get_Val4(void);
u16 TPAD_Get_MaxVal2(u8 n);
u16 TPAD_Get_MaxVal3(u8 n);
u16 TPAD_Get_MaxVal4(u8 n);
u8 TPAD_Scan0(u8 mode);
u8 TPAD_Scan1(u8 mode);
u8 TPAD_Scan2(u8 mode);
void TIM3_CH2_3_4_Cap_Init(u16 arr,u16 psc);

void adjustDefaultFunc(void);
void adjust_GATE_VAL2(MsgQueue_Def* queue);


/// @}

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////

