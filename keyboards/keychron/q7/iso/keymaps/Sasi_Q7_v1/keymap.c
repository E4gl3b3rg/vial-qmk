// source ~/qmk-venv/bin/activate
// qmk compile -kb keychron/q7/iso -km Sasi_Q7_v1
// qmk flash -kb keychron/q7/iso -km Sasi_Q7_v1

#include QMK_KEYBOARD_H
#include "common.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_iso_73(
        KC_ESC,  KC_1,     KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,     KC_0,     KC_MINS,  KC_EQL,  KC_BSPC,          KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,     KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                   KC_END,  KC_PGDN,
        KC_CAPS, KC_A,     KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,     NAV_SCLN, KC_QUOT,  KC_NUHS, KC_ENT,           KC_NO,   KC_NO,
        KC_LSFT, KC_NUBS,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,  KC_DOT,   KC_SLSH,           KC_RSFT,          KC_UP,   KC_NO,
        KC_LCTL, KC_LGUI,  KC_LALT,                            KC_SPC,                              KC_RALT,  CTRL_ESC, KC_RCTL, KC_NO,   KC_LEFT, KC_DOWN, KC_RGHT),

    // Loosely vim-based cursor and document navigation keys.
    // Accessed by holding semicolon.
    // Acts as cmd for any unbound keys.
    [_NAV] = LAYOUT_iso_73(
        _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______,  _______,  _______,  _______, _______, _______, _______,
        _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______,  _______,  _______,  _______,          _______, _______,
        _______, _______, _______,  _______, _______, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT, _______,  _______,  _______, _______, _______, _______,
        _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______,  _______,  _______,  _______,          _______, _______,
        _______, _______, _______,                             _______,                             _______,  _______,  _______, _______, _______, _______, _______),

};


// track the time of the last key input.
static uint16_t last_key_time;

// track the last keycode pressed.
static uint16_t last_key_code;

// A NAV_SCLN saját lenyomási ideje.
// Ezt nem írhatja felül a közben lenyomott másik billentyű.
static uint16_t nav_scln_time;

static bool nav_scln_tap_pending;

// melyik oprendszert használom
//static bool mac_mode;

// track the state of NAV_SCLN
// 0 - not activated
// 1 - pressed (waiting to decide on semicolon or nav)
// 2 - consumed (upgraded to semicolon or used in nav layer)
// 3 - consumed (as ctrl-tab rotation)
// 4 - consumed (as cmd)
static int semicolon_nav_activated;

// keep track of the current kvm target (to play a different sound on switch).
static int kvm_target;

// keep track of the kvm target which is in game mode.
static int game_target = -1;

#define IS_GAME kvm_target == game_target

// track the state of CTRL_ESC over multiple key presses.
// 0 - not activated
// 1 - pressed (esc activated, ctrl pending on next keystroke)
// 2 - pressed (ctrl activated, esc pending on CTRL_ESC release if no combo consumption)
// 3 - consumed (upgraded to ctrl or autoexited on special case keys)
static int ctrl_escape_activated;


// number of vim movement keys pressed in a row.
static int vim_movement;

static bool vim_insert;




#include "_unregister_nav_scln_down_state.c"
#include "_process_nav_scln.c"
#include "_process_ctrl_esc.c"



// Ez induláskor:
// kiolvassa a QMK user EEPROM-területét,
// megnézi az első bitet,
// ennek megfelelően beállítja a mac_mode változót.
//void keyboard_post_init_user(void) {
//    uint32_t user_config = eeconfig_read_user();
//
//    mac_mode = (user_config & OS_MODE_MAC) != 0;
//}


// Mac / PC mód kezelése.
// Az OS_TOGGLE billentyű megnyomásakor átváltunk a Mac és a PC mód között.
// A kiválasztott módot az EEPROM-ba is elmentjük, így a billentyűzet
// újraindítása vagy áramtalanítása után is megmarad a beállítás.
//bool process_os_toggle(uint16_t keycode, keyrecord_t *record) {
//    switch (keycode) {
//        case OS_TOGGLE:
//            if (record->event.pressed) {
//                mac_mode = !mac_mode;
//
//                eeconfig_update_user(mac_mode ? OS_MODE_MAC : 0);
//            }
//            return false;
//    }
//
//    return true;
//}



bool process_all_custom(uint16_t keycode, keyrecord_t *record) {
    //    if (!process_symbol_specials(keycode, record)) return false;

    //    if (!process_game_specials(keycode, record)) return false;

    // in game mode, all excess processing is skipped (mainly to avoid unwanted macro / helper triggers).
    if (!(IS_GAME))
    {
        //        if (!process_specials(keycode, record)) return false;

        // delay shift down presses until next key.
        //        if (!process_left_shift(keycode, record)) return false;
        //        if (!process_right_shift(keycode, record)) return false;

        if (!process_nav_scln(keycode, record)) return false;

        //        if (!process_backtick_surround(keycode, record)) return false;

        //        if (!process_meh(keycode, record)) return false;

        //        if (!process_macros(keycode, record)) return false;

        if (!process_ctrl_esc(keycode, record)) return false;
    }

    return true;
}


// For debounce and edge case purposes, track the last key timing, key code,
// and whether it was a numeric character or not.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    bool retval = process_all_custom(keycode, record);

    // // Mac / PC mód váltás kezelése.
    // if (!process_os_toggle(keycode, record)) return false;

    if (record->event.pressed)
    {
        last_key_time = timer_read();
        last_key_code = keycode;
    }

    // update_last_was_number(keycode, record);
    return retval;
}



// ============================================================================
// QMK funkciók – rövid súgó
// ============================================================================
//
// BILLENTYŰK KEZELÉSE
// ----------------------------------------------------------------------------
//
// register_code(KC_x)       // Billentyű lenyomása, felengedés nélkül.
// unregister_code(KC_x)     // Billentyű felengedése.
//
// register_code16(KC_x)     // Billentyű lenyomása 16 bites keycode-dal.
// unregister_code16(KC_x)   // 16 bites keycode-dal lenyomott billentyű felengedése.
// tap_code16(KC_x)          // Billentyű egyszeri megnyomása és felengedése.
//
//
// MÓDOSÍTÓK
// ----------------------------------------------------------------------------
//
// get_mods()                // Az aktuálisan aktív módosítók lekérdezése.
// MOD_BIT(KC_x)             // Az adott módosítóhoz tartozó bitmaszk.
// clear_mods()              // Az összes aktív módosító törlése.
//
//
// LAYER KEZELÉS
// ----------------------------------------------------------------------------
//
// layer_on(_LAYER)          // A megadott layer bekapcsolása.
// layer_off(_LAYER)         // A megadott layer kikapcsolása.
//
//
// IDŐZÍTÉS
// ----------------------------------------------------------------------------
//
// timer_read()              // Az aktuális QMK idő lekérése.
// timer_elapsed(timer)      // Az adott időpont óta eltelt idő lekérése.
//
//
// BILLENTYŰESEMÉNYEK
// ----------------------------------------------------------------------------
//
// process_record_user()     // A QMK billentyűesemény-feldolgozó hookja.
//                              Ha történik egy billentyűesemény, itt nézhetjük meg és dönthetjük el, mit csináljunk vele.
//                              keycode = Megmondja, melyik billentyűről van szó.
//                              record = A billentyűesemény további adatait tartalmazza.
//                              record->event.pressed = true → a billentyűt lenyomták | false → a billentyűt felengedték.
// record->event.pressed     // true = lenyomás, false = felengedés.
//
// ============================================================================


