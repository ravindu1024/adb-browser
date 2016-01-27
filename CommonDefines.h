#pragma once

#define VKEY_UP             0x41
#define VKEY_LEFT           0x44
#define VKEY_RIGHT          0x43
#define VKEY_DOWN           0x42
#define VKEY_Q              0x71
#define VKEY_ENTER          0xa
#define VKEY_BACKSPACE      0x7f
#define VKEY_K              0x6b
#define VKEY_V              0x76
#define VKEY_R              0x72
#define VKEY_C              0x63
#define VKEY_P              0x70
#define VKEY_ESC_CHAR       0x1b
#define VKEY_ESC_CHAR2      0x5b
#define VKEY_ESC_CHAR3      0x33
#define VKEY_DELETE         0x7e
#define VKEY_TAB            0x09
#define VKEY_A              0x61

#define HEADER_HEIGHT       4
#define STATUS_HEIGHT       4
#define WINDOW_MARGIN       9
#define MIN_ROWS            20
#define MIN_COLUMNS         40

#define ADB                 "adb"
#define PERMISSIONS_WIDTH   11
#define USERNAME_WIDTH      10
#define NAME_OFFSET         21



enum DeviceType
{
    DeviceTypex86,
    DeviceTypeAndroid
};
