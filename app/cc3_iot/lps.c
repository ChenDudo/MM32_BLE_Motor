// Define to prevent recursive inclusion  --------------------------------------
#define _LPS_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "lps.h"
#include "sensor_common.h"

bool LPS22HB_Init()
{
    HANDLE hI2C= CreateFile(emIP_I2C);
    if (hI2C == NULL)       while(1);

    tAPP_I2C_DCB dcb;
    sensor_I2C_Init(&dcb, LPS22HB_ADDR, LPS22HB_WHO_AM_I);

    u8 id;

    //检查LPS的ID
    OpenFile(hI2C, (void*)&dcb);
    if (ReadFile(hI2C, emFILE_I2C1, &id, 1) < 0)
        lpsRight = false;
    if (id != LPS22HB_ID) {
        lpsRight = false;
    }
    else lpsRight = true;

    //LPS初始化
    //设置模式为低功耗模式使能
    dcb.subAddr = LPS22HB_RES_CONF;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id |= 0x01;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    //配置ODR频率为0，关掉设备
    dcb.subAddr = LPS22HB_CTRL_REG1;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0x0F;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    //低通滤波器配置
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0xFB;
        id |= 0x08;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    //阻止数据更新开启，BDU位置1
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id |= 0x02;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    //关掉寄存器地址自增
    dcb.subAddr = LPS22HB_CTRL_REG2;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0xEF;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }

    //使能PRESSURE
    dcb.subAddr = LPS22HB_CTRL_REG1;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id |= 0x30;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    return lpsRight;
}

void LPS22HB_SetOutputDataRate(float odr)
{
    LPS22HB_ODR new_odr;

    new_odr = (odr <=  1.0f) ? LPS22HB_ODR_1Hz
            : (odr <= 10.0f) ? LPS22HB_ODR_10Hz
            : (odr <= 25.0f) ? LPS22HB_ODR_25Hz
            : (odr <= 50.0f) ? LPS22HB_ODR_50Hz
            : LPS22HB_ODR_75Hz;

    HANDLE hI2C= CreateFile(emIP_I2C);
    if (hI2C == NULL)       while(1);

    tAPP_I2C_DCB dcb;
    u8 id;
    sensor_I2C_Init(&dcb, LPS22HB_ADDR, LPS22HB_CTRL_REG1);

    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0x70;
        id |= (new_odr << 4);
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
}

void LPS22HB_Config()
{
    LPS22HB_SetOutputDataRate(10.0f);
}

double LPS22HB_Get_Press()
{
    u32 pressVal;
    double press;
    pressVal = Sensor_GetRegValue(LPS22HB_ADDR, LPS22HB_PRESS_OUT_XL);
    pressVal |= (Sensor_GetRegValue(LPS22HB_ADDR, LPS22HB_PRESS_OUT_L) << 8);
    pressVal |= (Sensor_GetRegValue(LPS22HB_ADDR, LPS22HB_PRESS_OUT_H) << 16);
    press = (double)pressVal / 4096;
    return press;
}
