////////////////////////////////////////////////////////////////////////////////
/// @file     STEP_MOTOR.C
/// @author   D Chen
/// @version  v2.0.0
/// @date     2019-05-21
/// @brief    THIS FILE PROVIDES ALL THE MOTOR EXAMPLE.
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
#define _STEP_MOTOR_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "drv.h"
#include "resource.h"
#include "motor.h"
#include "tb6612.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AppTaskTick()
{
    if (tickCnt++ >= 500) {
        tickCnt  = 0;
        tickFlag = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  set delay time
/// @param  ms: delay time
/// @retval bool: true: delay ms time
////////////////////////////////////////////////////////////////////////////////
bool delay(u16 ms)
{
    static u16 cnt = 0;
    if (cnt == 0) {
        cnt = ms;
    }
	else if (sysTickFlag1mS) {
        sysTickFlag1mS = false;
        if (cnt > 0)
            cnt--;
        if (cnt == 0)
            return true;
    }
    return false;
}

void (*fpStep)(void);
typedef void (*fp)();
fp stepInterface[4] = {
    step1of4,
    step2of4,
    step3of4,
    step4of4
};
emStepper_handle stepHandle;
emMotor_handle motor1;
u32 stepFreqMax = 1300;
u8  S_Flexible = 5;
u16 S_StartCnt = 0;
u16 S_Len = 400;
u16 stepLen = 10000;
bool S_Start;

#pragma location=0x0800e000
static const u16 sModeData[400] = {

517,517,518,518,518,519,519,519,520,520,
521,521,522,522,522,523,523,524,524,525,
525,526,526,527,527,528,528,529,530,530,
531,531,532,533,533,534,535,535,536,537,
537,538,539,540,540,541,542,543,544,545,
545,546,547,548,549,550,551,552,553,554,
555,556,557,558,559,560,562,563,564,565,
566,568,569,570,572,573,574,576,577,579,
580,581,583,585,586,588,589,591,593,594,
596,598,600,601,603,605,607,609,611,613,

615,617,619,621,623,625,627,630,632,634,
637,639,641,644,646,649,651,654,656,659,
662,664,667,670,673,676,679,681,684,687,
690,694,697,700,703,706,709,713,716,719,
723,726,730,733,737,740,744,748,751,755,
759,763,766,770,774,778,782,786,790,794,
798,802,806,811,815,819,823,827,832,836,
840,845,849,854,858,863,867,872,876,881,
885,890,895,899,904,908,913,918,922,927,
932,937,941,946,951,956,960,965,970,975,

979,984,989,993,998,1003,1008,1012,1017,1022,
1027,1031,1036,1041,1045,1050,1054,1059,1064,1068,
1073,1077,1082,1086,1091,1095,1100,1104,1109,1113,
1117,1122,1126,1130,1134,1138,1143,1147,1151,1155,
1159,1163,1167,1171,1175,1179,1183,1186,1190,1194,
1198,1201,1205,1209,1212,1216,1219,1223,1226,1230,
1233,1236,1240,1243,1246,1249,1252,1255,1259,1262,
1265,1268,1270,1273,1276,1279,1282,1285,1287,1290,
1293,1295,1298,1300,1303,1305,1308,1310,1312,1315,
1317,1319,1322,1324,1326,1328,1330,1332,1334,1336,

1338,1340,1342,1344,1346,1348,1349,1351,1353,1355,
1356,1358,1360,1361,1363,1364,1366,1368,1369,1370,
1372,1373,1375,1376,1377,1379,1380,1381,1383,1384,
1385,1386,1387,1389,1390,1391,1392,1393,1394,1395,
1396,1397,1398,1399,1400,1401,1402,1403,1404,1404,
1405,1406,1407,1408,1409,1409,1410,1411,1412,1412,
1413,1414,1414,1415,1416,1416,1417,1418,1418,1419,
1419,1420,1421,1421,1422,1422,1423,1423,1424,1424,
1425,1425,1426,1426,1427,1427,1427,1428,1428,1429,
1429,1430,1430,1430,1431,1431,1431,1432,1432,1432,

};

#include "mm32.h"
#include "hal_tim.h"
#if defined(__USE_STEPPER)
////////////////////////////////////////////////////////////////////////////////
void TIM14_IRQHandler()
{
    if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) {
        // change the Step Function index
        if (stepHandle.stepDir) {
            if (++stepHandle.stepIdx == 4)
                stepHandle.stepIdx = 0;
        }
        else {
            if (--stepHandle.stepIdx == -1)
                stepHandle.stepIdx = 3;
        }
        // run Step Function
        fpStep = stepInterface[stepHandle.stepIdx];
        fpStep();

        // if first run S-curve
        if(S_Start) {
            if (++S_StartCnt <= S_Len) {
                stepHandle.stepFreq = sModeData[S_StartCnt - 1];
                writeStepFreq(stepHandle.stepFreq);
            }
            else {
                S_StartCnt = 0;
                S_Start = false;
            }
        }
        if(--(stepHandle.stepSum) == 0)
            stepHandle.stepStart = false;
        // clear IT pending
        TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
    }
}
#endif


////////////////////////////////////////////////////////////////////////////////
void main(void)
{
    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);

    stepHandle.stepStart      = true       ;
    stepHandle.stepDir        = 0          ;
    stepHandle.stepIdx        = 0          ;
    stepHandle.stepPulseMax   = 100        ;
    stepHandle.stepPulseWidth = 100        ;
    stepHandle.stepSum        = stepLen    ;
    stepHandle.stepFreq       = 500        ;

    motor1.motorType  = emMotor_Step     ;
    motor1.motorSta   = emMotor_Standby  ;
    motor1.handle     = (int*)&stepHandle;

    initMotor(motor1);

    S_Start = true;
    writeStepPulseWidth(stepHandle.stepPulseWidth);

    while (1) {

        if (!stepHandle.stepStart) {
            stepHandle.stepPulseWidth = (u16)(stepHandle.stepPulseMax * 0.35);
            stepHandle.stepIdx = 0;
            //motorStandby(true);
            TIM14->CR1 &= ~0x1;
        }
        else {
            stepHandle.stepPulseWidth = (u16)(stepHandle.stepPulseMax * 1);
            motorStandby(false);
            TIM14->CR1 |= 0x1;
        }

        writeStepPulseWidth(stepHandle.stepPulseWidth);

        if (SysKeyboard(&vkKey)) {
            switch  (vkKey) {
                case  VK_K0:
                (vdLED & 0x01) ? (vdLED &= ~0x01) : (vdLED |= 0x01); // toggle LD3
                motorStandby(true);
                KeyProcess_Key0();
                break;
                case  VK_K1:
                (stepHandle.stepDir) ?  (stepHandle.stepDir = 0) : (stepHandle.stepDir = 1);
                KeyProcess_Key1();
                break;
                case  VK_K2:
                S_Start = true;
                S_StartCnt = 0;
                stepHandle.stepStart = true;
                stepHandle.stepSum   = stepLen;
                KeyProcess_Key2();
                break;
                case  VK_K3:
                stepHandle.stepStart = false;
                stepHandle.stepSum   = 0;
                KeyProcess_Key3();
                break;
                default:
                break;
            }
        }

        if (tickFlag) {
            (vdLED & 0x02) ? (vdLED &= ~0x02) : (vdLED |= 0x02); // toggle LD3
            tickFlag = false;
        }
        SysDisplay((u8*)&vdLED);
    }
}

/// @}

/// @}

/// @}
