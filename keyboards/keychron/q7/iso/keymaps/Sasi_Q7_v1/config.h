/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#define VIAL_KEYBOARD_UID {0xD8, 0xED, 0x39, 0x2E, 0xEB, 0xEE, 0x80, 0x37}

#define VIAL_UNLOCK_COMBO_ROWS { 0, 2 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 13 }




//#define NO_DEBUG
//#define NO_PRINT
//#define NO_USB_STARTUP_CHECK

#define MOUSEKEY_INERTIA
#define MOUSEKEY_DELAY 0
#define MOUSEKEY_INTERVAL 8
#define MOUSEKEY_TIME_TO_MAX 64
#define MOUSEKEY_MAX_SPEED 40
#define MOUSEKEY_FRICTION  1
#define MOUSEKEY_MOVE_DELTA 1

//#ifdef AUDIO_ENABLE
//  #define STARTUP_SONG SONG(S__NOTE(_E2))
//  #define GOODBYE_SONG SONG(S__NOTE(_C2))
//#endif

// for sanwa keyboard emulation KVM
//#undef NKRO_ENABLE

//#define USB_POLLING_INTERVAL_MS 5

