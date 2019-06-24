////////////////////////////////////////////////////////////////////////////////
/// @file     TB6612.H
/// @author   D Chen
/// @version  v2.0.0
/// @date     2019-05-21
/// @brief    THIS FILE PROVIDES ALL THE TB6612 DRIVER FUNCTIONS.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT 2018-2019 MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////

// Define to prevent recursive inclusion  --------------------------------------
#ifndef __TB6612_H
#define __TB6612_H

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup EXAMPLE_TIM
/// @brief TIM example modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup TIM_Exported_Variables
/// @{


////////////////////////////////////////////////////////////////////////////////
#define AIN1_Port    GPIOB
#define AIN1_Pin     GPIO_Pin_8

#define AIN2_Port    GPIOB
#define AIN2_Pin     GPIO_Pin_5

#define BIN1_Port    GPIOA
#define BIN1_Pin     GPIO_Pin_15

#define BIN2_Port    GPIOB
#define BIN2_Pin     GPIO_Pin_0

#define STBY_Port    GPIOB
#define STBY_Pin     GPIO_Pin_4

#define PWMA_Port    GPIOB
#define PWMA_Pin     GPIO_Pin_9

#define PWMB_Port    GPIOB
#define PWMB_Pin     GPIO_Pin_3

////////////////////////////////////////////////////////////////////////////////
typedef enum {
    emMotor_None,
    emMotor_DC,
    emMotor_Step
} emType_M;

typedef enum {
    emMotor_Standby,
    emMotor_Busy,
    emMotor_Err,
    emMotor_Run
} emSta_M;

typedef enum {
    emDir_CW,
    emDir_CCW
} emDir_M;

typedef struct {
    u8  stepStart;
    u8  stepDir;
    s8  stepIdx;
    u16 stepPulseMax;
    u16 stepPulseWidth;
    u32 stepSum;
    u32 stepFreq;
} emStepper_handle;

typedef enum {
    emDC_Standby,
    emDC_Run,
    emDC_Stop,
    emDC_Brake
} emSta_DC;

typedef struct {
    u8          dc1Dir;
    u8          dc2Dir;
    u16         dcPulseMax;
    u16         dc1PulseWidth;
    u16         dc2PulseWidth;
    emSta_DC    dc1Sta;
    emSta_DC    dc2Sta;
} emDC_handle;

typedef struct {
    emType_M    motorType;
    emSta_M     motorSta;
    int*        handle;
} emMotor_handle;

////////////////////////////////////////////////////////////////////////////////
#ifdef _TB6612_C_

#define GLOBAL

GLOBAL u32 tim14CntFreq;

#else
#define GLOBAL extern
#endif

GLOBAL bool stepRunFlag;
GLOBAL emDC_handle* dcHandle;

#undef GLOBAL

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup TIM_Exported_Functions
/// @{

// --- base common func --------------------------------------------------------
emSta_M initMotor(emMotor_handle m);

void motorStandby(u8 en);
void init6612Pin();
void initPWMA(u16 psc, u16 period, u16 pulse);
void initPWMB(u16 psc, u16 period, u16 pulse);
void writePWMA(u16 val);
void writePWMB(u16 val);


// --- stepper motor func ------------------------------------------------------
void stepperRun(emStepper_handle* s);
void writeStepFreq(u32 val);
void writeStepPulseWidth(u16 pulse);
void step1of4();
void step2of4();
void step3of4();
void step4of4();
void initStepFreq(u16 timPsc, u32 stepFreq);

float S_curve(u32 len, float fstart, float fstop, u8 flexible, u16 index);


// --- dc motor func -----------------------------------------------------------
void dcMotorRun(emDC_handle* dc);

void writeOutADir(u8 dir);
void writeOutBDir(u8 dir);
void writeOutAStop();
void writeOutBStop();
void writeOutABrake();
void writeOutBBrake();


// --- dc motor encoder func ---------------------------------------------------
void initECA();
void initECB();
/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /* __TB6612_H */
////////////////////////////////////////////////////////////////////////////////
