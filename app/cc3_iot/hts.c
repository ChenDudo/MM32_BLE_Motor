// Define to prevent recursive inclusion  --------------------------------------
#define _HTS_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "hts.h"
#include "sensor_common.h"

bool HTS221_Init()
{
    HANDLE hI2C= CreateFile(emIP_I2C);
    if (hI2C == NULL)       while(1);

    tAPP_I2C_DCB dcb;
    sensor_I2C_Init(&dcb, HTS221_ADDR, HTS221_WHO_AM_I);

    u8 id;

    //检查HTS的ID
    OpenFile(hI2C, (void*)&dcb);
    if (ReadFile(hI2C, emFILE_I2C1, &id, 1) < 0)
        htsRight = false;
    if (id != HTS221_ID) {
        htsRight = false;
    }
    else htsRight = true;

    //HTS初始化
    //关掉power，PD位置0
    dcb.subAddr = HTS221_CTRL_REG1;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0x7F;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    //阻止数据更新开启，BDU位置1
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id |= 0x04;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    //配置ODR频率为1Hz
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0xFD;
        id |= 0x01;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }

    //使能TEMP、HUM
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id |= 0x80;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    return htsRight;
}

void HTS221_Get_Equation_Temp()
{
    u8 T0_degC;
    u8 T1_degC;
    u8 T1_T0msb;
    float T0;
    float T1;
    s16 T0_OUT;
    s16 T1_OUT;

    T0_degC = Sensor_GetRegValue(HTS221_ADDR, 0x32);
    T1_degC = Sensor_GetRegValue(HTS221_ADDR, 0x33);
    T1_T0msb = Sensor_GetRegValue(HTS221_ADDR, 0x35);
    T0 = (float)(T0_degC | ((T1_T0msb & 0x03) << 8)) / 8;
    T1 = (float)(T1_degC | ((T1_T0msb & 0x0C) << 6)) / 8;

    T0_OUT = Sensor_GetRegValue(HTS221_ADDR, 0x3C);
    T0_OUT |= (Sensor_GetRegValue(HTS221_ADDR, 0x3D) << 8);
    T1_OUT = Sensor_GetRegValue(HTS221_ADDR, 0x3E);
    T1_OUT |= (Sensor_GetRegValue(HTS221_ADDR, 0x3F) << 8);

    k_Temp = (float)(T1 - T0) / (T1_OUT - T0_OUT);
    b_Temp = (float)T1 - k_Temp * T1_OUT;
}

float HTS221_Get_AVTemp()
{
    s16 temp;
    temp = Sensor_GetRegValue(HTS221_ADDR, HTS221_TEMP_OUT_L);
    temp += (Sensor_GetRegValue(HTS221_ADDR, HTS221_TEMP_OUT_H) << 8);
    return (float)(temp * k_Temp + b_Temp);
}

void HTS221_Get_Equation_Hum()
{
    u8 H0_rH;
    u8 H1_rH;
    s16 H0_T0out;
    s16 H1_T0out;

    H0_rH = Sensor_GetRegValue(HTS221_ADDR, 0x30);
    H1_rH = Sensor_GetRegValue(HTS221_ADDR, 0x31);
    H0_T0out = Sensor_GetRegValue(HTS221_ADDR, 0x36);
    H0_T0out |= (Sensor_GetRegValue(HTS221_ADDR, 0x37) << 8);
    H1_T0out = Sensor_GetRegValue(HTS221_ADDR, 0x3A);
    H1_T0out |= (Sensor_GetRegValue(HTS221_ADDR, 0x3B) << 8);

    k_Hum = (float)(H1_rH - H0_rH) / (H1_T0out - H0_T0out);
    b_Hum = (float)H1_rH - k_Hum * H1_T0out;
}

float HTS221_Get_AVHum()
{
    s16 hum;
    hum = Sensor_GetRegValue(HTS221_ADDR, HTS221_HUMIDITY_OUT_L);
    hum += (Sensor_GetRegValue(HTS221_ADDR, HTS221_HUMIDITY_OUT_H) << 8);
    return (float)(hum * k_Hum + b_Hum) / 2;
}
