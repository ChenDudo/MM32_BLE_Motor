// Define to prevent recursive inclusion  --------------------------------------
#define _ADC_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "common.h"
#include "bsp.h"
#include "adc.h"
#include "hal_adc.h"
#include "hal_gpio.h"


////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void adcTick()
{
    
}

////////////////////////////////////////////////////////////////////////////////
void initADC()
{
    COMMON_EnableIpClock(emCLOCK_ADC1);
    
}
