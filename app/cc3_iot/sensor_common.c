// Define to prevent recursive inclusion  --------------------------------------
#define _SENSOR_COMMON_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "drv.h"
#include "sensor_common.h"

void sensor_I2C_Init(tAPP_I2C_DCB *pDcb, u8 slave, u16 sub)
{
    pDcb->hSub      = emFILE_I2C1;
    pDcb->type      = emTYPE_Polling;
    pDcb->block     = emTYPE_Block;
    pDcb->sync      = emTYPE_Sync;
    pDcb->remapEn   = true;
    pDcb->remapIdx  = 1;
    pDcb->timeOut   = 0;
    pDcb->fast      = true;
    pDcb->master    = true;
    pDcb->slave     = slave;
    pDcb->subAddr   = sub;
    pDcb->subSize   = 1;
    pDcb->ownaddr   = 0x20;
}


u8 Sensor_GetRegValue(u8 sensorAddr, u8 regAddr)
{
    HANDLE hI2C= CreateFile(emIP_I2C);
    if (hI2C == NULL)       while(1);

    tAPP_I2C_DCB dcb;
    u8 id;
    sensor_I2C_Init(&dcb, sensorAddr, regAddr);
    OpenFile(hI2C, (void*)&dcb);
    ReadFile(hI2C, emFILE_I2C1, &id, 1);
    return id;
}
