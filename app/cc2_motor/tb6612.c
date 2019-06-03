////////////////////////////////////////////////////////////////////////////////
/// @file     TB6612.C
/// @author   D Chen
/// @version  v2.0.0
/// @date     2019-05-20
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
#define _TB6612_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include <math.h>

#include "system_mm32.h"
#include "common.h"
#include "bsp.h"

#include "hal_tim.h"
#include "hal_gpio.h"
#include "hal_rcc.h"

#include "tb6612.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup EXAMPLE_TIM
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup TIM_Exported_Functions
/// @{

////////////////////////////////////////////////////////////////////////////////
void init6612Pin()
{
    COMMON_EnableIpClock(emCLOCK_GPIOA);
    COMMON_EnableIpClock(emCLOCK_GPIOB);

    GPIO_ResetBits(AIN1_Port, AIN1_Pin);      // MAN   =>   AIN1
    GPIO_ResetBits(AIN2_Port, AIN2_Pin);      // MAP   =>   AIN2
    GPIO_ResetBits(BIN1_Port, BIN1_Pin);      // MBN   =>   BIN2
    GPIO_ResetBits(BIN2_Port, BIN2_Pin);      // MBP   =>   BIN1
    GPIO_ResetBits(PWMA_Port, PWMA_Pin);      // PWMA
    GPIO_ResetBits(PWMB_Port, PWMB_Pin);      // PWMB
    GPIO_ResetBits(STBY_Port, STBY_Pin);      // Standby Mode: 0 active

    GPIO_Mode_OUT_PP_Init(AIN1_Port, AIN1_Pin);   // MAN   =>   AIN1
    GPIO_Mode_OUT_PP_Init(AIN2_Port, AIN2_Pin);   // MAP   =>   AIN2
    GPIO_Mode_OUT_PP_Init(BIN1_Port, BIN1_Pin);   // MBN   =>   BIN2
    GPIO_Mode_OUT_PP_Init(BIN2_Port, BIN2_Pin);   // MBP   =>   BIN1
    GPIO_Mode_OUT_PP_Init(STBY_Port, STBY_Pin);   // STBY
}

////////////////////////////////////////////////////////////////////////////////
void initPWMA(u16 psc, u16 period, u16 pulse)
{
    COMMON_EnableIpClock(emCLOCK_TIM17);
    GPIO_Mode_AF_PP_20MHz_Init(PWMA_Port, PWMA_Pin, NO_REMAP, GPIO_AF_2);

    TIM_TimeBaseInitTypeDef pBase = {
        .TIM_Prescaler         = psc,                   // 0 ~ 0xFFFF
        .TIM_Period            = period,                // 0 ~ 0xFFFF(some TIMx support 0xFFFFFFF)
        .TIM_ClockDivision     = TIM_CKD_DIV1,
        .TIM_CounterMode       = TIM_CounterMode_Up,    /// if use \TIM_CounterMode_CenterAligned3, PWM period is A half
        .TIM_RepetitionCounter = 0
    };
    TIM_TimeBaseInit(TIM17, &pBase);

    TIM_OCInitTypeDef pOC = {
        .TIM_Pulse        = pulse,               // 0 ~ 0xFFFF(some TIMx support 0xFFFFFFF)
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,
        .TIM_OCPolarity   = TIM_OCPolarity_High,
        .TIM_OCNPolarity  = TIM_OCNPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Set,
        .TIM_OCNIdleState = TIM_OCNIdleState_Reset
    };
    TIM_OC1Init(TIM17, &pOC);

    TIM_ARRPreloadConfig(TIM17, ENABLE);
    TIM_OC1PreloadConfig(TIM17, TIM_OCPreload_Enable);

    //COMMON_NVIC_Configure(TIM17_IRQn,  1,  1);
    //TIM_ITConfig(TIM17, TIM_IT_CC1, ENABLE);

    TIM_CtrlPWMOutputs(TIM17, ENABLE);
    TIM_Cmd(TIM17, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void initPWMB(u16 psc, u16 period, u16 pulse)
{
    COMMON_EnableIpClock(emCLOCK_TIM2);
    GPIO_Mode_AF_PP_20MHz_Init(PWMB_Port, PWMB_Pin, NO_REMAP, GPIO_AF_2);

    TIM_TimeBaseInitTypeDef pBase = {
        .TIM_Prescaler         = psc,                   // 0 ~ 0xFFFF
        .TIM_Period            = period,                // 0 ~ 0xFFFF(some TIMx support 0xFFFFFFF)
        .TIM_ClockDivision     = TIM_CKD_DIV1,
        .TIM_CounterMode       = TIM_CounterMode_Up,    /// if use \TIM_CounterMode_CenterAligned3, PWM period is A half
        .TIM_RepetitionCounter = 0
    };
    TIM_TimeBaseInit(TIM2, &pBase);

    TIM_OCInitTypeDef pOC = {
        .TIM_Pulse        = pulse,               // 0 ~ 0xFFFF(some TIMx support 0xFFFFFFF)
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,
        .TIM_OCPolarity   = TIM_OCPolarity_High,
        .TIM_OCNPolarity  = TIM_OCNPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Set,
        .TIM_OCNIdleState = TIM_OCNIdleState_Reset
    };
    TIM_OC2Init(TIM2, &pOC);

    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

    //COMMON_NVIC_Configure(TIM2_IRQn,  1,  1);
    //TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void writePWMA(u16 val)
{
    TIM_SetCompare1(TIM17, val);
}

////////////////////////////////////////////////////////////////////////////////
void writePWMB(u16 val)
{
    TIM_SetCompare2(TIM2, val);
}

////////////////////////////////////////////////////////////////////////////////
void writeStepFreq(u32 val)
{
    u16 arr = (u16)(tim14CntFreq / val - 1);
    TIM_SetAutoreload(TIM14, arr);
}

////////////////////////////////////////////////////////////////////////////////
void writeStepPulseWidth(u16 pulse)
{
    writePWMA(pulse);
    writePWMB(pulse);
}

////////////////////////////////////////////////////////////////////////////////
void motorStandby(u8 en)
{
    (en) ? GPIO_ResetBits(STBY_Port, STBY_Pin) : GPIO_SetBits(STBY_Port, STBY_Pin);
}

////////////////////////////////////////////////////////////////////////////////
void step1of4()
{
    //step 0: AB: 1010
    GPIO_SetBits(AIN1_Port, AIN1_Pin);
    GPIO_ResetBits(AIN2_Port, AIN2_Pin);
    GPIO_SetBits(BIN1_Port, BIN1_Pin);
    GPIO_ResetBits(BIN2_Port, BIN2_Pin);
}

////////////////////////////////////////////////////////////////////////////////
void step2of4()
{
    //step 1: ~AB: 0110
    GPIO_ResetBits(AIN1_Port, AIN1_Pin);
    GPIO_SetBits(AIN2_Port, AIN2_Pin);
    GPIO_SetBits(BIN1_Port, BIN1_Pin);
    GPIO_ResetBits(BIN2_Port, BIN2_Pin);
}
////////////////////////////////////////////////////////////////////////////////
void step3of4()
{
    //step 2: ~A~B: 0101
    GPIO_ResetBits(AIN1_Port, AIN1_Pin);
    GPIO_SetBits(AIN2_Port, AIN2_Pin);
    GPIO_ResetBits(BIN1_Port, BIN1_Pin);
    GPIO_SetBits(BIN2_Port, BIN2_Pin);
}
////////////////////////////////////////////////////////////////////////////////
void step4of4()
{
    //step 3: A~B: 1001
    GPIO_SetBits(AIN1_Port, AIN1_Pin);
    GPIO_ResetBits(AIN2_Port, AIN2_Pin);
    GPIO_ResetBits(BIN1_Port, BIN1_Pin);
    GPIO_SetBits(BIN2_Port, BIN2_Pin);
}

////////////////////////////////////////////////////////////////////////////////
u8 stepIdx;
void (*fpStep)(void);
typedef void (*fp)();
fp stepInterface[4] = {
    step1of4,
    step2of4,
    step3of4,
    step4of4
};

////////////////////////////////////////////////////////////////////////////////
void initStepFreq(u16 timPsc, u32 stepFreq)
{
    u32 timclk;
    RCC_ClocksTypeDef rcc_clocks;

    COMMON_EnableIpClock(emCLOCK_TIM14);

    RCC_GetClocksFreq(&rcc_clocks);
    if (rcc_clocks.PCLK1_Frequency == rcc_clocks.HCLK_Frequency)
        timclk = rcc_clocks.PCLK1_Frequency;
    else
        timclk = rcc_clocks.PCLK1_Frequency * 2 / rcc_clocks.HCLK_Frequency;

    tim14CntFreq = timclk / (timPsc + 1);
    u16 arr = (u16)(tim14CntFreq / stepFreq - 1);

    TIM_TimeBaseInitTypeDef pBase = {
        .TIM_Prescaler         = timPsc,                // 0 ~ 0xFFFF, counter_Freq = APBx_Freq / (timPsc + 1)
        .TIM_Period            = arr,                   // 0 ~ 0xFFFF(some TIMx support 0xFFFFFFF)
        .TIM_ClockDivision     = TIM_CKD_DIV1,
        .TIM_CounterMode       = TIM_CounterMode_Up,    /// if use \TIM_CounterMode_CenterAligned3, PWM period is A half
        .TIM_RepetitionCounter = 0
    };
    TIM_TimeBaseInit(TIM14, &pBase);

    TIM_ARRPreloadConfig(TIM14, ENABLE);

    COMMON_NVIC_Configure(TIM14_IRQn,  1,  1);
    TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
    TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM14, ENABLE);
}


////////////////////////////////////////////////////////////////////////////////
void writeOutADir(u8 dir)
{
    if (dir) {
        GPIO_SetBits    (AIN1_Port, AIN1_Pin);  // MAN   =>   AIN1
        GPIO_ResetBits  (AIN2_Port, AIN2_Pin);  // MAP   =>   AIN2
    }
    else {
        GPIO_ResetBits  (AIN1_Port, AIN1_Pin);  // MAN   =>   AIN1
        GPIO_SetBits    (AIN2_Port, AIN2_Pin);  // MAP   =>   AIN2
    }
}

////////////////////////////////////////////////////////////////////////////////
void writeOutBDir(u8 dir)
{
    if (dir) {
        GPIO_SetBits    (BIN1_Port, BIN1_Pin);  // MBP   =>   BIN1
        GPIO_ResetBits  (BIN2_Port, BIN2_Pin);  // MBN   =>   BIN2
    }
    else {
        GPIO_ResetBits  (BIN1_Port, BIN1_Pin);  // MBP   =>   BIN1
        GPIO_SetBits    (BIN2_Port, BIN2_Pin);  // MBN   =>   BIN2
    }
}

////////////////////////////////////////////////////////////////////////////////
void writeOutAStop()
{
    GPIO_ResetBits(AIN1_Port, AIN1_Pin);
    GPIO_ResetBits(AIN2_Port, AIN2_Pin);
}

////////////////////////////////////////////////////////////////////////////////
void writeOutBStop()
{
    GPIO_ResetBits(BIN1_Port, BIN1_Pin);
    GPIO_ResetBits(BIN2_Port, BIN2_Pin);
}

////////////////////////////////////////////////////////////////////////////////
void writeOutABrake()
{
    GPIO_SetBits(AIN1_Port, AIN1_Pin);
    GPIO_SetBits(AIN2_Port, AIN2_Pin);
}

////////////////////////////////////////////////////////////////////////////////
void writeOutBBrake()
{
    GPIO_SetBits(BIN1_Port, BIN1_Pin);
    GPIO_SetBits(BIN2_Port, BIN2_Pin);
}

////////////////////////////////////////////////////////////////////////////////
float S_curve(u32 len, float fstart, float fstop, u8 flexible, u16 index)
{
    if(index > len) index = len;

    float num = len / 2;
    float melo = flexible * (index - num) / num;
    float deno = 1.0 / (1 + exp(-melo));

    return (fstart - (fstart - fstop) * deno);
}


////////////////////////////////////////////////////////////////////////////////
emSta_M initMotor(emMotor_handle m)
{
    init6612Pin();
    if (emMotor_Step == m.motorType) {
        emStepper_handle s;
        memcpy(&s, m.handle, sizeof(emStepper_handle));
        initPWMA(0, s.stepPulseMax, s.stepPulseWidth);
        initPWMB(0, s.stepPulseMax, s.stepPulseWidth);
        initStepFreq(7, s.stepFreq);
        return emMotor_Standby;
    }
    else if (emMotor_DC == m.motorType) {
        emDC_handle dc;
        memcpy(&dc, m.handle, sizeof(emDC_handle));
        initPWMA(0, dc.dcPulseMax, dc.dc1PulseWidth);
        initPWMB(0, dc.dcPulseMax, dc.dc1PulseWidth);
        writeOutADir(dc.dc1Dir);
        writeOutBDir(dc.dc2Dir);
        return emMotor_Standby;
    }
    else
        return emMotor_Err;
}


////////////////////////////////////////////////////////////////////////////////
void stepperRun(emStepper_handle* s)
{
    if (stepRunFlag) {
        motorStandby(false);
        if (!s->stepStart) {
            s->stepPulseWidth = (u16)(s->stepPulseMax * 0.05);
            s->stepIdx = 0;
            motorStandby(true);
        }
        else {
            if (s->stepDir) {
                if (++s->stepIdx == 4)
                    s->stepIdx = 0;
            }
            else {
                if (--s->stepIdx == -1)
                    s->stepIdx = 3;
            }
            if(--(s->stepSum) == 0)
                s->stepStart = false;

        }
        writeStepPulseWidth(s->stepPulseWidth);
        writeStepFreq(s->stepFreq);
        fpStep = stepInterface[s->stepIdx];
        fpStep();

        stepRunFlag = false;
    }
}


////////////////////////////////////////////////////////////////////////////////
void dcMotorRun(emDC_handle* dc)
{
    if ((dc->dc1Sta == emDC_Standby) || (dc->dc2Sta == emDC_Standby))
        motorStandby(true);
    else {
        motorStandby(false);

        switch (dc->dc1Sta) {
            case emDC_Run:
            writeOutADir(dc->dc1Dir);
            break;
            case emDC_Stop:
            writeOutAStop();
            break;
            case emDC_Brake:
            writeOutABrake();
            break;
            default:
            break;
        }
        switch (dc->dc2Sta) {
            case emDC_Run:
            writeOutBDir(dc->dc2Dir);
            break;
            case emDC_Stop:
            writeOutBStop();
            break;
            case emDC_Brake:
            writeOutBBrake();
            break;
            default:
            break;
        }
    }
    writePWMA(dc->dc1PulseWidth);
    writePWMB(dc->dc2PulseWidth);
}

#if defined(__USE_STEPPER)
////////////////////////////////////////////////////////////////////////////////
void TIM14_IRQHandler()
{
    if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
        stepRunFlag = true;
    }
}
#endif

/// @}

/// @}

/// @}
