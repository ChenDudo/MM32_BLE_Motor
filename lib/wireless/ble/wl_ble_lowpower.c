////////////////////////////////////////////////////////////////////////////////
/// @file     WL_BLE_LOWPOWER.C
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-02-18
/// @brief    THIS FILE PROVIDES ALL THE BLE_VARIABLES EXAMPLE.
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
#define _WL_BLE_LOWPOWER_C_

#include "types.h"
#include "MM32.h"

#include "HAL_rcc.h"
#include "HAL_gpio.h"

#include "wl_ble_lowpower.h"

// -----------------------------------------------------------------------------
// ---- SYSTEM CLOCK FOR LOWPOWER ----------------------------------------------
// -----------------------------------------------------------------------------
void SetSystemClock_HSI()
{
//	unsigned char temp=0;
//
	RCC->CR|=RCC_CR_HSION;
	while(!(RCC->CR&RCC_CR_HSIRDY));
	RCC->CFGR=RCC_CFGR_PPRE1_DIV2; //APB1=DIV2;APB2=DIV1;AHB=DIV1;

    FLASH->ACR=FLASH_ACR_LATENCY_1|FLASH_ACR_PRFTBE;	  //FLASH 2 delay clk cycles

	RCC->CFGR &=~ RCC_CFGR_SW;
	RCC->CFGR |=  RCC_CFGR_SW_HSI;//PLL to be the sys clock
}

void SetSystemClock_HSI_6d()
{
//	unsigned char temp=0;
//
	RCC->CFGR &=~ RCC_CFGR_SW;
	RCC->CFGR |=  RCC_CFGR_SW_HSI_DIV6;//PLL to be the sys clock
}

void SystemClock_Init(void)
{
    SetSystemClock_HSI();
}

void SysClk48to8(void)
{
//    SetSystemClock_HSI_6d();
//	SysTick_Config(8000);
}

void SysClk8to48(void)
{
//    SetSystemClock_HSI();
//	SysTick_Config(48000);
}

// -----------------------------------------------------------------------------
// ---- LOWPOWER ---------------------------------------------------------------
// -----------------------------------------------------------------------------
void McuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
//	if (lowpower_mode && (tx_timeout < SysTick_Count) && (rx_timeout < SysTick_Count)) {
//		if(lowpower_mode == 1) {//sleep
//			SCB->SCR &= 0xfb;
//			__WFE();
//		} else { //stop
//			SysClk48to8();
//			SCB->SCR |= 0x4;
//
//			__WFI();
//
//			RCC->CR|=RCC_CR_HSION;
//			SysClk8to48();
//
//			GPIO_ResetBits(GPIOB, GPIO_Pin_7);
//		}
//	}
}

void DisableEnvINT(void)
{
	//to disable interrupt
	__ASM volatile("cpsid i");

	interrupt_diable_cnt ++;
}

void EnableEnvINT(void)
{
	//to enable/recover interrupt
	interrupt_diable_cnt --;
	if(interrupt_diable_cnt <= 0) //protection purpose
	{
		interrupt_diable_cnt = 0; //reset
		__ASM volatile("cpsie i");
	}
}