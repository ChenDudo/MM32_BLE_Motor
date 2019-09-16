////////////////////////////////////////////////////////////////////////////////
/// @file     BLE.C
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-02-18
/// @brief    THIS FILE PROVIDES ALL THE BLE EXAMPLE.
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
#define _BLE_C_

// Includes  -------------------------------------------------------------------
#include "string.h"
#include "system_mm32.h"
#include "resource.h"

#include "wl_hw.h"
#include "wl_ble_services.h"
#include "wl_ble_lowpower.h"

#include "mg_api.h"
#include "ble.h"

u32 irq_count = 0;
void wl_ble_irq_handler()
{
    if (ble_running_flag) {
        irq_count++;
        ble_run(0);
    }
}

void wl_ble_mode()
{
    ble_running_flag = false;
    u8 device_name[] = "MMNJ LogoWall";

//    lowpower_mode = 0x02;
    cur_notifyhandle = 19;

    memcpy(ble_device_name, device_name, sizeof(device_name));

    wl_spi_init();
    wl_irq_it_init();

    SetBleIntRunningMode();

    radio_initBle(TXPWR_0DBM, &ble_mac_addr);

    SysTick_Count = 0;
    while(SysTick_Count < 5);

    ble_run_interrupt_start(160*2);
    ble_running_flag = true;
    
    //ble_set_adv_type(0);
    
//    u8 adv_pack[] = {0x02, 0x01, 0x06, 0x03, 0x03, 0xE0, 0xFE};
    u8 adv_pack[] = {0x02, 0x01, 0x06, 0x05, 0x03, 0x15, 0x11, 0x1F, 0x11};
    ble_set_adv_data(adv_pack, strlen(adv_pack));
}
u32 tick_count = 0;
void wl_ble_tick_task()
{
    static u32 tx_led_count = 0;
    static u32 rx_led_count = 0;

    SysTick_Count++;

    if (ble_running_flag) {
        ble_nMsRoutine();
        tick_count++;
    }

    if (ble_tx_led) {
        ble_tx_led = false;
        vdLED |= 0x04;
        SysDisplay(&vdLED);
        tx_led_count = SysTick_Count + 100;
    }

    if (ble_rx_led) {
        ble_rx_led = false;
        vdLED |= 0x08;
        SysDisplay(&vdLED);
        rx_led_count = SysTick_Count + 100;
    }

    if (tx_led_count < SysTick_Count) {
        vdLED &=~ 0x04;
        SysDisplay(&vdLED);
    }

    if (rx_led_count < SysTick_Count) {
        vdLED &=~ 0x08;
        SysDisplay(&vdLED);
    }
}

void wl_ble_task()
{
    ble_run(160*2);
}


uint32_t GetSysTickCount(void) 	// porting api
{
    return SysTick_Count;
}

void ConnectStausUpdate(unsigned char IsConnectedFlag) //porting api
{
    //[IsConnectedFlag] indicates the connection status

	if ((bool)IsConnectedFlag != connected_flag)
	{
		connected_flag = IsConnectedFlag;

//        if(connected_flag) {
//            lowpower_mode = 1;
//        } else {
//            lowpower_mode = 2;
//        }
	}
}

// -----------------------------------------------------------------------------
// ---- USER CALLBACK ----------------------------------------------------------
// -----------------------------------------------------------------------------
// As central device
void UsrProcCallback_Central(u8 fin, u8* dat_rcv, u8 dat_len)
{
    u8 i = 0;
    i++;
}

// As peri
void UsrProcCallback()
{
    u8 i = 0;
    i++;

}


