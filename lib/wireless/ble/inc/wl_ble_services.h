////////////////////////////////////////////////////////////////////////////////
/// @file     WL_BLE_SERVICES.H
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-02-18
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE WL_BLE_SERVICES
///           EXAMPLES.
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
#ifndef __WL_BLE_SERVICES_H
#define __WL_BLE_SERVICES_H

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup EXAMPLE_WL_BLE_SERVICES
/// @brief WL_BLE_SERVICES example modules
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @defgroup WL_BLE_SERVICES_Exported_Constants
/// @{

// GATT Services
// https://www.bluetooth.com/specifications/gatt/services/

// GATT Characteristic
// https://www.bluetooth.com/specifications/gatt/characteristics/

// GATT Declarations
// https://www.bluetooth.com/specifications/gatt/declarations/

// GATT Descriptors
// https://www.bluetooth.com/specifications/gatt/descriptors/

#define ATT_CHAR_PROP_BDCAST                            0x01                    // Broadcast
#define ATT_CHAR_PROP_RD                                0x02                    // Read
#define ATT_CHAR_PROP_W_NORSP                           0x04                    // Write without Response
#define ATT_CHAR_PROP_W                                 0x08                    // Write
#define ATT_CHAR_PROP_NTF                               0x10                    // Notify
#define ATT_CHAR_PROP_IND                               0x20                    // Indicate


// GATT PROFILE ATTRIBUTE TYPES
#define ATT_TYPE_PRIMARY                                0x2800                  // Primary Service
#define ATT_TYPE_INC                                    0x2802                  // Include
#define ATT_TYPE_CHAR                                   0x2803                  // Characteristic
#define ATT_TYPE_INFO                                   0x2901                  // Characteristic User Description
#define ATT_TYPE_CFG                                    0x2902                  // Client Characteristic Configuration
#define ATT_TYPE_xRpRef                                 0x2907                  // External Report Reference
#define ATT_TYPE_RpRef                                  0x2908                  // Report Reference

// ATT Error Codes
#define ATT_ERR_INVALID_HANDLE                          0x01
#define ATT_ERR_READ_NOT_PERMITTED                      0x02
#define ATT_ERR_WRITE_NOT_PERMITTED                     0x03
#define ATT_ERR_INVALID_PDU                             0x04
#define ATT_ERR_INSUFFICIENT_AUTHEN                     0x05
#define ATT_ERR_UNSUPPORTED_REQ                         0x06
#define ATT_ERR_INVALID_OFFSET                          0x07
#define ATT_ERR_INSUFFICIENT_AUTHOR                     0x08
#define ATT_ERR_PREPARE_QUEUE_FULL                      0x09
#define ATT_ERR_ATTR_NOT_FOUND                          0x0a
#define ATT_ERR_ATTR_NOT_LONG                           0x0b
#define ATT_ERR_INSUFFICIENT_KEY_SIZE                   0x0c
#define ATT_ERR_INVALID_VALUE_SIZE                      0x0d
#define ATT_ERR_UNLIKELY                                0x0e
#define ATT_ERR_INSUFFICIENT_ENCRYPT                    0x0f
#define ATT_ERR_UNSUPPORTED_GRP_TYPE                    0x10
#define ATT_ERR_INSUFFICIENT_RESOURCES                  0x11

#define UUID_TYPE_16BIT                                 0xFF                    // Bluetooth SIG UUID (16-bit).
#define UUID_TYPE_128BIT                                0x00                    // Vendor UUID types start at this index (128-bit).

// Assigned Values for BLE UUIDs
#define BLE_UUID_UNKNOWN                                0x0000
#define BLE_UUID_SERVICE_PRIMARY                        0x2800
#define BLE_UUID_SERVICE_SECONDARY                      0x2801
#define BLE_UUID_SERVICE_INCLUDE                        0x2802
#define BLE_UUID_CHARACTERISTIC                         0x2803
#define BLE_UUID_DESCRIPTOR_CHAR_EXT_PROP               0x2900
#define BLE_UUID_DESCRIPTOR_CHAR_USER_DESC              0x2901
#define BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG          0x2902
#define BLE_UUID_DESCRIPTOR_SERVER_CHAR_CONFIG          0x2903
#define BLE_UUID_DESCRIPTOR_CHAR_PRESENTATION_FORMAT    0x2904
#define BLE_UUID_DESCRIPTOR_CHAR_AGGREGATE_FORMAT       0x2905
#define BLE_UUID_GATT                                   0x1801
#define BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED    0x2A05
#define BLE_UUID_GAP                                    0x1800
#define BLE_UUID_GAP_CHARACTERISTIC_DEVICE_NAME         0x2A00
#define BLE_UUID_GAP_CHARACTERISTIC_APPEARANCE          0x2A01
#define BLE_UUID_GAP_CHARACTERISTIC_PPF                 0x2A02
#define BLE_UUID_GAP_CHARACTERISTIC_RECONN_ADDR         0x2A03
#define BLE_UUID_GAP_CHARACTERISTIC_PPCP                0x2A04
#define BLE_UUID_GAP_CHARACTERISTIC_CAR                 0x2AA6
#define BLE_UUID_GAP_CHARACTERISTIC_RPA_ONLY            0x2AC9

// Bluetooth Appearance values
#define BLE_APPEARANCE_UNKNOWN                              0
#define BLE_APPEARANCE_GENERIC_PHONE                        64
#define BLE_APPEARANCE_GENERIC_COMPUTER                     128
#define BLE_APPEARANCE_GENERIC_WATCH                        192
#define BLE_APPEARANCE_WATCH_SPORTS_WATCH                   193
#define BLE_APPEARANCE_GENERIC_CLOCK                        256
#define BLE_APPEARANCE_GENERIC_DISPLAY                      320
#define BLE_APPEARANCE_GENERIC_REMOTE_CONTROL               384
#define BLE_APPEARANCE_GENERIC_EYE_GLASSES                  448
#define BLE_APPEARANCE_GENERIC_TAG                          512
#define BLE_APPEARANCE_GENERIC_KEYRING                      576
#define BLE_APPEARANCE_GENERIC_MEDIA_PLAYER                 640
#define BLE_APPEARANCE_GENERIC_BARCODE_SCANNER              704
#define BLE_APPEARANCE_GENERIC_THERMOMETER                  768
#define BLE_APPEARANCE_THERMOMETER_EAR                      769
#define BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR            832
#define BLE_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT    833
#define BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE               896
#define BLE_APPEARANCE_BLOOD_PRESSURE_ARM                   897
#define BLE_APPEARANCE_BLOOD_PRESSURE_WRIST                 898
#define BLE_APPEARANCE_GENERIC_HID                          960
#define BLE_APPEARANCE_HID_KEYBOARD                         961
#define BLE_APPEARANCE_HID_MOUSE                            962
#define BLE_APPEARANCE_HID_JOYSTICK                         963
#define BLE_APPEARANCE_HID_GAMEPAD                          964
#define BLE_APPEARANCE_HID_DIGITIZERSUBTYPE                 965
#define BLE_APPEARANCE_HID_CARD_READER                      966
#define BLE_APPEARANCE_HID_DIGITAL_PEN                      967
#define BLE_APPEARANCE_HID_BARCODE                          968
#define BLE_APPEARANCE_GENERIC_GLUCOSE_METER                1024
#define BLE_APPEARANCE_GENERIC_RUNNING_WALKING_SENSOR       1088
#define BLE_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE       1089
#define BLE_APPEARANCE_RUNNING_WALKING_SENSOR_ON_SHOE       1090
#define BLE_APPEARANCE_RUNNING_WALKING_SENSOR_ON_HIP        1091
#define BLE_APPEARANCE_GENERIC_CYCLING                      1152
#define BLE_APPEARANCE_CYCLING_CYCLING_COMPUTER             1153
#define BLE_APPEARANCE_CYCLING_SPEED_SENSOR                 1154
#define BLE_APPEARANCE_CYCLING_CADENCE_SENSOR               1155
#define BLE_APPEARANCE_CYCLING_POWER_SENSOR                 1156
#define BLE_APPEARANCE_CYCLING_SPEED_CADENCE_SENSOR         1157
#define BLE_APPEARANCE_GENERIC_PULSE_OXIMETER               3136
#define BLE_APPEARANCE_PULSE_OXIMETER_FINGERTIP             3137
#define BLE_APPEARANCE_PULSE_OXIMETER_WRIST_WORN            3138
#define BLE_APPEARANCE_GENERIC_WEIGHT_SCALE                 3200
#define BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACT           5184
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_DISP          5185
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_AND_NAV_DISP  5186
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_POD           5187
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_AND_NAV_POD   5188

typedef struct ble_character {
    u16 attribute_type;                                                         // Attribute Types
    u16 attribute_handle;                                                       // Attribute Handle
    u8  character_properties;                                                   // Attribute Value: Characteristic Properties
    u8  character_handle[2];                                                    //                  Characteristic Value Attribute Handle
    u8  character_uuid[2];                                                      //                  Characteristic UUID
    union character_uuid_type
    {
        u8 uuid_type;                                                           // if character_uuid is 16: 0xFF
        u8 uuid_128_idx;
    };
} BLE_CHAR;

typedef struct ble_uuid_128 {
    u8  uuid_128[16];                                                            //uuid128 string: little endian
} BLE_UUID_128;

/// @}


////////////////////////////////////////////////////////////////////////////////
/// @defgroup WL_BLE_SERVICES_Exported_Variables
/// @{
#ifdef _WL_BLE_SERVICES_C_
#define GLOBAL

#else
#define GLOBAL extern
#endif

#define BUF_SIZE 			100

GLOBAL u8  ble_tx_buf[BUF_SIZE];
GLOBAL u8  ble_rx_buf[BUF_SIZE];

GLOBAL char     ble_device_name[24];     // max len is 24 bytes
GLOBAL u16      cur_notifyhandle;

GLOBAL bool     ble_tx_led;
GLOBAL bool     ble_rx_led;
GLOBAL bool     bleWriteFlag;

#undef GLOBAL


/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup WL_BLE_SERVICES_Exported_Functions
/// @{

/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /* WL_BLE_SERVICES_H */
////////////////////////////////////////////////////////////////////////////////

