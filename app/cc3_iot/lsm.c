// Define to prevent recursive inclusion  --------------------------------------
#define _LSM_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "lsm.h"
#include "sensor_common.h"

bool LSM6DSL_Init()
{
    HANDLE hI2C= CreateFile(emIP_I2C);
    if (hI2C == NULL)       while(1);

    tAPP_I2C_DCB dcb;
    sensor_I2C_Init(&dcb, LSM6DSL_ADDR, LSM6DSL_WHO_AM_I);

    u8 id;

    //检查LSM的ID
    OpenFile(hI2C, (void*)&dcb);
    if (ReadFile(hI2C, emFILE_I2C1, &id, 1) < 0)
        lsmRight = false;
    if (id != LSM6DSL_ID) {
        lsmRight = false;
    }
    else lsmRight = true;

    //LSM初始化
    //寄存器地址自增使能
    dcb.subAddr = LSM6DSL_CTRL3_C;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id |= 0x04;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    //阻止数据更新开启，BDU位置1
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id |= 0x40;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    //选择FIFO模式
    dcb.subAddr = LSM6DSL_FIFO_CTRL5;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0x78;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    //加速度ODR设为0 - power down
    dcb.subAddr = LSM6DSL_CTRL1_XL;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0x0F;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    //加速度选择 - 00: 2g
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0xF3;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    //陀螺仪ODR设为0 - power down
    dcb.subAddr = LSM6DSL_CTRL2_G;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0x0E;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    //陀螺仪选择 - 2000dps
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0xFC;
        id |= 0x0C;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }

    //使能GYRO,ODR设为104Hz
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0x0E;
        id |= 0x40;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }

    //使能ACC,ODR设为104Hz
    dcb.subAddr = LSM6DSL_CTRL1_XL;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0x0E;
        id |= 0x40;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
    return lsmRight;
}

void LSM6DSL_ACC_SetOutputDataRate(float odr)
{
    HANDLE hI2C= CreateFile(emIP_I2C);
    if (hI2C == NULL)       while(1);

    tAPP_I2C_DCB dcb;
    sensor_I2C_Init(&dcb, LSM6DSL_ADDR, LSM6DSL_CTRL1_XL);

    u8 id;
    LSM6DSL_ODR_XL new_odr;

    new_odr = (odr <=   12.5f) ? LSM6DSL_XL_ODR_12Hz5
            : (odr <=   26.0f) ? LSM6DSL_XL_ODR_26Hz
            : (odr <=   52.0f) ? LSM6DSL_XL_ODR_52Hz
            : (odr <=  104.0f) ? LSM6DSL_XL_ODR_104Hz
            : (odr <=  208.0f) ? LSM6DSL_XL_ODR_208Hz
            : (odr <=  416.0f) ? LSM6DSL_XL_ODR_416Hz
            : (odr <=  833.0f) ? LSM6DSL_XL_ODR_833Hz
            : (odr <= 1660.0f) ? LSM6DSL_XL_ODR_1k66Hz
            : (odr <= 3330.0f) ? LSM6DSL_XL_ODR_3k33Hz
            : LSM6DSL_XL_ODR_6k66Hz;

    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0x0F;
        id |= (new_odr << 4);
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }

}

void LSM6DSL_ACC_SetFullScale(u8 fullScale)
{
    HANDLE hI2C= CreateFile(emIP_I2C);
    if (hI2C == NULL)       while(1);

    tAPP_I2C_DCB dcb;
    sensor_I2C_Init(&dcb, LSM6DSL_ADDR, LSM6DSL_CTRL1_XL);

    u8 id;
    LSM6DSL_FS_XL new_fs;

    new_fs = (fullScale <= 2) ? LSM6DSL_2g
           : (fullScale <= 4) ? LSM6DSL_4g
           : (fullScale <= 8) ? LSM6DSL_8g
           : LSM6DSL_16g;

    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0xF3;
        id |= (new_fs << 2);
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
}

void LSM6DSL_6D_threshold_set(LSM6DSL_SIXD_THS val)
{
    HANDLE hI2C= CreateFile(emIP_I2C);
    if (hI2C == NULL)       while(1);

    tAPP_I2C_DCB dcb;
    sensor_I2C_Init(&dcb, LSM6DSL_ADDR, LSM6DSL_TAP_THS_6D);

    u8 id;

    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id &= 0x1F;
        id |= (val << 5);
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
}

void LSM6DSL_Pin_Int1_Route_Get(u8* int1Route)
{
    HANDLE hI2C= CreateFile(emIP_I2C);
    if (hI2C == NULL)       while(1);

    tAPP_I2C_DCB dcb;
    sensor_I2C_Init(&dcb, LSM6DSL_ADDR, LSM6DSL_INT1_CTRL);

    u8 id;

    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0)
        int1Route[0] = id;

    dcb.subAddr = LSM6DSL_MD1_CFG;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0)
        int1Route[1] = id;

    dcb.subAddr = LSM6DSL_CTRL4_C;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0)
        int1Route[2] = id;

    dcb.subAddr = LSM6DSL_MASTER_CONFIG;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0)
        int1Route[3] = id;
}

void LSM6DSL_Pin_Int1_Route_Set(u8* int1Route)
{
    HANDLE hI2C= CreateFile(emIP_I2C);
    if (hI2C == NULL)       while(1);

    tAPP_I2C_DCB dcb;
    sensor_I2C_Init(&dcb, LSM6DSL_ADDR, LSM6DSL_INT1_CTRL);

    u8 id;

    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id = int1Route[0];
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }

    dcb.subAddr = LSM6DSL_MD1_CFG;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id = (int1Route[1] | 0x04);
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }

    dcb.subAddr = LSM6DSL_CTRL4_C;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id = int1Route[2];
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }

    dcb.subAddr = LSM6DSL_MASTER_CONFIG;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id = int1Route[3];
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }

    dcb.subAddr = LSM6DSL_TAP_CFG;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id |= 0x80;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
}

void LSM6DSL_Pin_Int2_Route_Get(u8* int2Route)
{
    HANDLE hI2C= CreateFile(emIP_I2C);
    if (hI2C == NULL)       while(1);

    tAPP_I2C_DCB dcb;
    sensor_I2C_Init(&dcb, LSM6DSL_ADDR, LSM6DSL_INT2_CTRL);

    u8 id;

    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0)
        int2Route[0] = id;

    dcb.subAddr = LSM6DSL_MD2_CFG;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0)
        int2Route[1] = id;

    dcb.subAddr = LSM6DSL_DRDY_PULSE_CFG_G;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0)
        int2Route[2] = id;
}

void LSM6DSL_Pin_Int2_Route_Set(u8* int2Route)
{
    HANDLE hI2C= CreateFile(emIP_I2C);
    if (hI2C == NULL)       while(1);

    tAPP_I2C_DCB dcb;
    sensor_I2C_Init(&dcb, LSM6DSL_ADDR, LSM6DSL_INT2_CTRL);

    u8 id;

    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id = int2Route[0];
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }

    dcb.subAddr = LSM6DSL_MD2_CFG;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id = (int2Route[1] | 0x04);
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }

    dcb.subAddr = LSM6DSL_DRDY_PULSE_CFG_G;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        id = (int2Route[2] | (id & 0x80));
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }

    dcb.subAddr = LSM6DSL_TAP_CFG;
    OpenFile(hI2C, (void*)&dcb);
    if(ReadFile(hI2C, emFILE_I2C1, &id, 1) > 0) {
        if ((int2Route[1] & 0xFC) != 0x00U)
            id |= 0x80;
        else id &= 0x7F;
        WriteFile(hI2C, emFILE_I2C1, &id, 1);
    }
}

void LSM6DSL_ACC_6D_Orientation_Enable(LSM6DSL_INTPIN intPin)
{
    LSM6DSL_ACC_SetOutputDataRate(416.0f);
    LSM6DSL_ACC_SetFullScale(8);
    LSM6DSL_6D_threshold_set(LSM6DSL_DEG_60);

    if (intPin == LSM6DSL_INT1_PIN) {
        LSM6DSL_Pin_Int1_Route_Get(int1Route);
        LSM6DSL_Pin_Int1_Route_Set(int1Route);
    }
    else {
        LSM6DSL_Pin_Int2_Route_Get(int2Route);
        LSM6DSL_Pin_Int2_Route_Set(int2Route);
    }
}

void LSM6DSL_6DOrientation_Init()
{
    LSM6DSL_Init();
    LSM6DSL_ACC_6D_Orientation_Enable(LSM6DSL_INT1_PIN);
}

void Send_Orientation()
{
    u8 xl = 0;
    u8 xh = 0;
    u8 yl = 0;
    u8 yh = 0;
    u8 zl = 0;
    u8 zh = 0;
    u8 val = 0;

    val = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_D6D_SRC);
    xl = val & 0x01;
    xh = val & 0x02;
    yl = val & 0x04;
    yh = val & 0x08;
    zl = val & 0x10;
    zh = val & 0x20;

    if (xl == 0U && yl == 0U && zl == 0U && xh == 0U && yh == 1U && zh == 0U) {
//        (void)snprintf(dataOut, MAX_BUF_SIZE,   "\r\n  ________________  " \
//                                                "\r\n |                | " \
//                                                "\r\n |  *             | " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |________________| \r\n");
    }

    else if (xl == 1U && yl == 0U && zl == 0U && xh == 0U && yh == 0U && zh == 0U) {
//        (void)snprintf(dataOut, MAX_BUF_SIZE,   "\r\n  ________________  " \
//                                                "\r\n |                | " \
//                                                "\r\n |             *  | " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |________________| \r\n");
    }

    else if (xl == 0U && yl == 0U && zl == 0U && xh == 1U && yh == 0U && zh == 0U) {
//        (void)snprintf(dataOut, MAX_BUF_SIZE,   "\r\n  ________________  " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |  *             | " \
//                                                "\r\n |________________| \r\n");
    }

    else if (xl == 0U && yl == 1U && zl == 0U && xh == 0U && yh == 0U && zh == 0U) {
//        (void)snprintf(dataOut, MAX_BUF_SIZE,   "\r\n  ________________  " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |                | " \
//                                                "\r\n |             *  | " \
//                                                "\r\n |________________| \r\n");
    }

    else if (xl == 0U && yl == 0U && zl == 0U && xh == 0U && yh == 0U && zh == 1U) {
//        (void)snprintf(dataOut, MAX_BUF_SIZE,   "\r\n  __*_____________  " \
//                                                "\r\n |________________| \r\n");
    }

    else if (xl == 0U && yl == 0U && zl == 1U && xh == 0U && yh == 0U && zh == 0U) {
//        (void)snprintf(dataOut, MAX_BUF_SIZE,   "\r\n  ________________  " \
//                                                "\r\n |________________| " \
//                                                "\r\n    *               \r\n");
    }
}

void LSM6DSL_Get_Event_Status()
{
//    u8 wake_up_src;
//    u8 tap_src;
//    u8 d6d_src;
//    u8 func_src;
    u8 md1_cfg;
    u8 md2_cfg;
    u8 int1_ctrl;

//    wake_up_src = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_WAKE_UP_SRC);
//    tap_src     = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_TAP_SRC);
//    d6d_src     = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_D6D_SRC);
//    func_src    = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_FUNC_SRC1);
    md1_cfg     = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_MD1_CFG);
    md2_cfg     = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_MD2_CFG);
    int1_ctrl   = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_INT1_CTRL);

    if ((md1_cfg & 0x10) || (md2_cfg & 0x10)) {
        //自由落体
    }
    if ((md1_cfg & 0x20) || (md2_cfg & 0x20)) {
        //唤醒
    }
    if ((md1_cfg & 0x80) || (md2_cfg & 0x80)) {
        //不活跃模式
    }
    if ((md1_cfg & 0x40) || (md2_cfg & 0x40)) {
        //单击
    }
    if ((md1_cfg & 0x08) || (md2_cfg & 0x08)) {
        //双击
    }
    if ((md1_cfg & 0x04) || (md2_cfg & 0x04)) {
        //6轴
    }
    if ((md1_cfg & 0x02) || (md2_cfg & 0x02)) {
        //倾斜
    }
    if (int1_ctrl & 0x80) {
        //计步已启用
    }
}

void LSM6DSL_6DOrientation_Process()
{
    Send_Orientation();
    LSM6DSL_Get_Event_Status();
    Send_Orientation();

//    HANDLE hI2C= CreateFile(emIP_I2C);
//  if (hI2C == NULL)       while(1);
//
//    tAPP_I2C_DCB dcb;
//    u8 id;
//    sensor_I2C_Init(&dcb, LSM6DSL_ADDR, LSM6DSL_CTRL2_G);
//
//    id = 0x1C;
//    OpenFile(hI2C, (void*)&dcb);
//    WriteFile(hI2C, emFILE_I2C1, &id, 1);
//
//    dcb.subAddr = LSM6DSL_CTRL10_C;
//    id = 0x38;
//    OpenFile(hI2C, (void*)&dcb);
//    WriteFile(hI2C, emFILE_I2C1, &id, 1);
//
//    dcb.subAddr = LSM6DSL_CTRL1_XL;
//    id = 0x60;
//    OpenFile(hI2C, (void*)&dcb);
//    WriteFile(hI2C, emFILE_I2C1, &id, 1);
//
//    dcb.subAddr = LSM6DSL_TAP_CFG;
//    id = 0x90;
//    OpenFile(hI2C, (void*)&dcb);
//    WriteFile(hI2C, emFILE_I2C1, &id, 1);
//
//    dcb.subAddr = LSM6DSL_WAKE_UP_DUR;
//    id = 0x00;
//    OpenFile(hI2C, (void*)&dcb);
//    WriteFile(hI2C, emFILE_I2C1, &id, 1);
//
//    dcb.subAddr = LSM6DSL_WAKE_UP_THS;
//    id = 0x02;
//    OpenFile(hI2C, (void*)&dcb);
//    WriteFile(hI2C, emFILE_I2C1, &id, 1);
//
//    dcb.subAddr = LSM6DSL_MD1_CFG;
//    id = 0x20;
//    OpenFile(hI2C, (void*)&dcb);
//    WriteFile(hI2C, emFILE_I2C1, &id, 1);
//
//    dcb.subAddr = LSM6DSL_TAP_THS_6D;
//    id = 0x40;
//    OpenFile(hI2C, (void*)&dcb);
//    WriteFile(hI2C, emFILE_I2C1, &id, 1);
//
//    dcb.subAddr = LSM6DSL_CTRL8_XL;
//    id = 0x01;
//    OpenFile(hI2C, (void*)&dcb);
//    WriteFile(hI2C, emFILE_I2C1, &id, 1);
}

float LSM6DSL_GYRO_GetSensitivity()
{
    u8 full_scale;
    full_scale = (Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_CTRL2_G) & 0x0C) >> 1;
    switch (full_scale) {
    case LSM6DSL_125dps:
        return LSM6DSL_GYRO_SENSITIVITY_FS_125DPS;
    case LSM6DSL_250dps:
        return LSM6DSL_GYRO_SENSITIVITY_FS_250DPS;
    case LSM6DSL_500dps:
        return LSM6DSL_GYRO_SENSITIVITY_FS_500DPS;
    case LSM6DSL_1000dps:
        return LSM6DSL_GYRO_SENSITIVITY_FS_1000DPS;
    case LSM6DSL_2000dps:
        return LSM6DSL_GYRO_SENSITIVITY_FS_2000DPS;
    default:
        return false;
    }
}

void Get_Angular_Rate(LSM6DSL_AXES* angularRate)
{
    s16 x, y, z;
    float sensitivity;
    sensitivity = LSM6DSL_GYRO_GetSensitivity();

    x = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_OUTX_L_G);
    x |= (Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_OUTX_H_G) << 8);
    y = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_OUTY_L_G);
    y |= (Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_OUTY_H_G) << 8);
    z = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_OUTZ_L_G);
    z |= (Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_OUTZ_H_G) << 8);

    angularRate->x = (s32)(float)((float)x * sensitivity);
    angularRate->y = (s32)(float)((float)y * sensitivity);
    angularRate->z = (s32)(float)((float)z * sensitivity);
}

float LSM6DSL_ACC_GetSensitivity()
{
    u8 full_scale;
    full_scale = (Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_CTRL1_XL) & 0x0C) >> 2;
    switch (full_scale) {
    case LSM6DSL_2g:
        return LSM6DSL_ACC_SENSITIVITY_FS_2G;
    case LSM6DSL_4g:
        return LSM6DSL_ACC_SENSITIVITY_FS_4G;
    case LSM6DSL_8g:
        return LSM6DSL_ACC_SENSITIVITY_FS_8G;
    case LSM6DSL_16g:
        return LSM6DSL_ACC_SENSITIVITY_FS_16G;
    default:
        return false;
    }
}

void Get_Acceleration_Rate(LSM6DSL_AXES* accRate)
{
    s16 x, y, z;
    float sensitivity;
    sensitivity = LSM6DSL_ACC_GetSensitivity();

    x = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_OUTX_L_XL);
    x |= (Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_OUTX_H_XL) << 8);
    y = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_OUTY_L_XL);
    y |= (Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_OUTY_H_XL) << 8);
    z = Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_OUTZ_L_XL);
    z |= (Sensor_GetRegValue(LSM6DSL_ADDR, LSM6DSL_OUTZ_H_XL) << 8);

    accRate->x = (s32)(float)((float)x * sensitivity);
    accRate->y = (s32)(float)((float)y * sensitivity);
    accRate->z = (s32)(float)((float)z * sensitivity);
}

