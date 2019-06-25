////////////////////////////////////////////////////////////////////////////////
/// @file     WL_BLE_SERVICES.C
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-02-18
/// @brief    THIS FILE PROVIDES ALL THE BLE_SERVICE EXAMPLE.
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
#define _WL_BLE_SERVICES_C_

#include "string.h"

#include "types.h"
#include "mg_api.h"
#include "wl_ble_services.h"

const BLE_CHAR AttCharList[] = {
    // Service: Generic Access 0x1800
    //Type          Attribute Handle    Characteristic Properties,              Characteristic Handle,  Characteristic UUID
    {ATT_TYPE_CHAR,     3,               ATT_CHAR_PROP_RD,                       4,     0,              0x00,   0x2a,           UUID_TYPE_16BIT},// Device Name
    {ATT_TYPE_CHAR,     5,               ATT_CHAR_PROP_RD,                       6,     0,              0x01,   0x2a,           UUID_TYPE_16BIT},// Appearance

    //Service: Device Information 0x180A
    {ATT_TYPE_CHAR,     8,               ATT_CHAR_PROP_RD,                        9,    0,              0x29,   0x2a,           UUID_TYPE_16BIT},// Manufacturer Name String
    {ATT_TYPE_CHAR,     10,              ATT_CHAR_PROP_RD,                       11,    0,              0x26,   0x2a,           UUID_TYPE_16BIT},// Firmware Revision String
    {ATT_TYPE_CHAR,     12,              ATT_CHAR_PROP_RD,                       13,    0,              0x28,   0x2a,           UUID_TYPE_16BIT},// Software Revision String

//    //Type          Attribute Handle    Characteristic Properties,              Characteristic Handle,  Characteristic UUID     Index for UUID 128 Bit
    {ATT_TYPE_CHAR,     18,               ATT_CHAR_PROP_NTF,                     19,    0,              0,0,                    1}, //RxNotify
    {ATT_TYPE_CFG,      20,               ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W},                                                        //Configuration
    {ATT_TYPE_CHAR,     21,               ATT_CHAR_PROP_W|ATT_CHAR_PROP_W_NORSP, 22,    0,              0,0,                    2}, //Tx
};


const BLE_UUID_128 AttUuid128List[] = {
//      0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15
    {0x36, 0x50, 0x8a, 0xc6, 0x5f, 0x24, 0xf8, 0x9d, 0x04, 0x4c, 0x0a, 0xfc, 0x00, 0x00, 0x00, 0x00},   // Service UUID
    {0x36, 0x50, 0x8a, 0xc6, 0x5f, 0x24, 0xf8, 0x9d, 0x04, 0x4c, 0x0a, 0xfc, 0x01, 0x00, 0x00, 0x00},   // Characteristic UUID
    {0x36, 0x50, 0x8a, 0xc6, 0x5f, 0x24, 0xf8, 0x9d, 0x04, 0x4c, 0x0a, 0xfc, 0x02, 0x00, 0x00, 0x00},   // Characteristic UUID
};

u8 GetCharListDim(void)
{
    return sizeof(AttCharList)/sizeof(AttCharList[0]);
}

// -----------------------------------------------------------------------------
// ---- Service Declear ----------------------------------------------------------
// -----------------------------------------------------------------------------
// --- Service Declear ---------------------------------------------------------
void att_server_rdByGrType( u8 pdu_type, u8 attOpcode, u16 st_hd, u16 end_hd, u16 att_type )
{
    if((att_type == ATT_TYPE_PRIMARY) && (st_hd == 1))
    {
        u8 service[] = {0x00, 0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type, 1, 6, service, 2);
        return;
    }

    else if((att_type == ATT_TYPE_PRIMARY) && (st_hd <= 7)) {
        u8 service[] = {0x0A, 0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type, 7, 15, service, 2);
        return;
    }

    else if((att_type == ATT_TYPE_PRIMARY) && (st_hd <= 16))
    {
        att_server_rdByGrTypeRspPrimaryService(pdu_type, 16, 22, (u8*)AttUuid128List[0].uuid_128, 16);
        return;
    }

    att_notFd( pdu_type, attOpcode, st_hd);
}

// Primary Service
int GetPrimaryServiceHandle(unsigned short hd_start, unsigned short hd_end,
                            unsigned short uuid16,
                            unsigned short* hd_start_r,unsigned short* hd_end_r)
{
    return 0;
}


// --- Client Write ------------------------------------------------------------
// Write response
void ser_write_rsp(u8 pdu_type, u8 attOpcode, u16 att_hd, u8* attValue, u8 valueLen_w)
{
    ble_rx_led = true;

    if(att_hd == 22) {
        bleWriteFlag = true;
        memcpy(ble_rx_buf, attValue, valueLen_w);
        ser_write_rsp_pkt(pdu_type);
        return;
    }

    if(att_hd == 19 || att_hd == 20 ) {
        ser_write_rsp_pkt(pdu_type);
        return;
    }

    att_notFd( pdu_type, attOpcode, att_hd);	/*the default response, also for the purpose of error robust */
}

// Write Long Characteristic Values
// Data is greater than 20 bytes
void ser_prepare_write(u16 handle, u8* attValue, u16 attValueLen, u16 att_offset)//user's call back api
{

}

// Execute write long characteristic values
void ser_execute_write(void)//user's call back api
{

}

// --- Client Read -------------------------------------------------------------
// Read response
// att_server_rd
void server_rd_rsp(u8 attOpcode, u16 attHandle, u8 pdu_type)
{
    ble_tx_led = true;

    // Device Name
    if (attHandle == 4) {
        att_server_rd( pdu_type, attOpcode, attHandle, (u8*)ble_device_name, strlen(ble_device_name)); //ble lib build version
        return;
    }

    // Appearance
    if (attHandle == 6) {
        u8 appearance[] = {0x80, 0x02};
        att_server_rd( pdu_type, attOpcode, attHandle, (u8*)appearance, sizeof(appearance) - 1); //ble lib build version
        return;
    }

    if (attHandle == 9) {
        att_server_rd( pdu_type, attOpcode, attHandle, get_ble_version(), strlen((const char*)get_ble_version())); //ble lib build version
        return;
    }

    if (attHandle == 11) {
        att_server_rd( pdu_type, attOpcode, attHandle, GetFirmwareInfo(),strlen((const char*)GetFirmwareInfo())); //ble lib build version
        return;
    }

    if (attHandle == 13) {
        u8 software_info[] = "SV2.0.1";
        att_server_rd( pdu_type, attOpcode, attHandle, (u8*)software_info, sizeof(software_info) - 1); //ble lib build version
        return;
    }

    if (attHandle == 20) {
        u8 config_info[2] = {0,0};
        att_server_rd( pdu_type, attOpcode, attHandle, (u8*)config_info, 2); //ble lib build version
        return;
    }

    att_notFd( pdu_type, attOpcode, attHandle );/*the default response, also for the purpose of error robust */
}

// Read Long Characteristic Values
// Response for read blob request
void server_blob_rd_rsp(u8 attOpcode, u16 attHandle, u8 dataHdrP, u16 offset)
{
    att_notFd( dataHdrP, attOpcode, attHandle);/*the default response, also for the purpose of error robust */
}

// --- Server Notify -----------------------------------------------------------
// Set handle for notify:
// void set_notifyhandle(u16 hd); or u16 cur_notifyhandle

// Notify funtion:
// u8 sconn_notifydata(u8* data, u8 len);

// Notify by callback
#include "mm32.h"
#include "uart.h"
void gatt_user_send_notify_data_callback(void)
{
//    ble_tx_led = true;
//
//    static u8 data[] = {0x12, 0x34};
//    sconn_notifydata(data, sizeof(data));
//
//    data[0]++;
//    data[1]++;
    
    if (recFlag) {
        sconn_notifydata(uartRxBuf, strlen(uartRxBuf));
        memset(uartRxBuf, 0x00, sizeof(uartRxBuf));
        recFlag = false;
    }
}

u8* getDeviceInfoData(u8* len)
{
    *len = sizeof(ble_device_name);
    return (u8*)ble_device_name;
}