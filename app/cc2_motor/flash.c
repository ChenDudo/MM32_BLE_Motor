// Define to prevent recursive inclusion  --------------------------------------
#define _FLASH_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include <math.h>
#include "system_mm32.h"

#include "flash.h"
#include "hal_flash.h"

#include "motor.h"
#include "tb6612.h"

#define FLASHADDR 0x0800E000

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void flashTick()
{
    u16* pReadValue = exFLASH_ReadEE(FLASHADDR, 2);
    u16 pwmValueRsv = *pReadValue;
    if((pReadValue == 0) || (pwmValueRsv > 100)) {
        u16 tempValue = PWM_DEFAULT_NUM;
        exFLASH_WriteEE(&tempValue, FLASHADDR, 2);    // set default value
        return;
    }
    // if first Open = true, using the reserved PWM value.
    if(isFirstOpen) {
        pwmSetValue = pwmValueRsv;
        isFirstOpen = false;
    }
    // check PWM value, compare & write data per 2s
    if(flashTickCnt ++ >= 2000) {
        flashTickCnt = 0;
        // if |pwmValueRsv - pwmSetValue| < 5, neednot reserved the PWM value.
        if(fabs(pwmValueRsv - pwmSetValue) > 5) {
            exFLASH_WriteEE(&pwmSetValue, FLASHADDR, 2);
        }
    }
}
