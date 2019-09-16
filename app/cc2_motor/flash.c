// Define to prevent recursive inclusion  --------------------------------------
#define _FLASH_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "common.h"
#include "bsp.h"
#include "flash.h"
#include "hal_adc.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_flash.h"

#include "tb6612.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void flashTick()
{


	if (flashTickCnt ++ >= 80) {
		flashTickCnt = 0;

	}
}
