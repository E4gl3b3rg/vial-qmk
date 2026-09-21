#include QMK_KEYBOARD_H
#include "common.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY] = LAYOUT_iso_73(
        KC_ESC,  KC_1,     KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,     KC_0,     KC_MINS,  KC_EQL,  KC_BSPC,          KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,     KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                   KC_END,  KC_PGDN,
        KC_CAPS, KC_A,     KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,     NAV_SCLN, KC_QUOT,  KC_NUHS, KC_ENT,           KC_NO,   KC_NO,
        KC_LSFT, KC_NUBS,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,  KC_DOT,   KC_SLSH,           KC_RSFT,          KC_UP,   KC_NO,
        KC_LCTL, KC_LGUI,  KC_LALT,                            KC_SPC,                              KC_RALT,  CTRL_ESC, KC_RCTL, KC_NO,   KC_LEFT, KC_DOWN, KC_RGHT),

    [_NAV] = LAYOUT_iso_73(
        _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______,  _______,  _______,  _______, _______, _______, _______,
        _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______,  _______,  _______,  _______,          _______, _______,
        _______, _______, _______,  _______, _______, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT, _______,  _______,  _______, _______, _______, _______,
        _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______,  _______,  _______,  _______,          _______, _______,
        _______, _______, _______,                             _______,                             _______,  _______,  _______, _______, _______, _______, _______),

};

// KC_SCLN

// track the time of the last key input.
static uint16_t last_key_time;






// track the state of NAV_SCLN
// 0 - not activated
// 1 - pressed (waiting to decide on semicolon or nav)
// 2 - consumed (upgraded to semicolon or used in nav layer)
// 3 - consumed (as ctrl-tab rotation)
static int semicolon_nav_activated;

bool process_nav_scln(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case NAV_SCLN:
            if (record->event.pressed) { // lenyomás logika
                // timer case here is just to give more immediacy to semicolons when typing at EOL.
                // set low enough to not impede intended navigation
                if (get_mods() & MOD_BIT(KC_LSFT) || timer_elapsed(last_key_time) < 180)
                {
                    register_code16(KC_SCLN);
                    semicolon_nav_activated = 2;
                    return true;
                }

                semicolon_nav_activated = 1;
                layer_on(_NAV);
                return false;
            }
            else // felengedési logika
            {
                switch (semicolon_nav_activated)
                {
                    case 1:
                        // clears a potential ctrl modifier from CTRL_ESC.
                        // sequence of events is SCLN down - wait 500ms - CTRL_ESC down - SCLN up - CTRL_ESC up
                        clear_mods();
                        tap_code16(KC_SCLN);
                        break;
                    case 2:
                        unregister_code16(KC_SCLN);
                        break;
                    case 3:
                        unregister_code16(KC_LSFT);
                        break;
                }

                semicolon_nav_activated = 0;
                layer_off(_NAV);
                return false;
            }
        case KC_UP:
        case KC_DOWN:
        case KC_ENT:
        case KC_PGUP:
        case KC_PGDN:
        case KC_LSFT:
        case KC_RSFT:
//         case WORD_L:
//         case WORD_R:
//         case LINE_L:
//         case LINE_R:
        case KC_LCTL:
//         case TAB_L:
//         case TAB_R:
//         case CLOSE_W:
            if (semicolon_nav_activated == 1)
            {
                semicolon_nav_activated = 2;
                return true;
            }
        case CTRL_ESC:
            // note that last_key_time will not be updated from the SCLN_NAV keypress itself.
            // this handles cases like SCLN_NAV -> KC_ESC rapidly after a previous character.
            if (semicolon_nav_activated == 1 && timer_elapsed(last_key_time) < 250)
            {
                tap_code16(KC_SCLN);
                semicolon_nav_activated = 2;
            }
            break;
        default:
            if (semicolon_nav_activated == 1)
            {
                tap_code16(KC_SCLN);
                semicolon_nav_activated = 2;
            }
            break;

        case KC_LEFT:
            if (!record->event.pressed || semicolon_nav_activated == 0)
                break;

            if (get_mods() & MOD_BIT(KC_LCTL))
            {
                register_code16(KC_LSFT);
                tap_code16(KC_TAB);
                semicolon_nav_activated = 3;
                return false;
            }
            else
                semicolon_nav_activated = 2;
            break;

        case KC_RGHT:
            if (!record->event.pressed || semicolon_nav_activated == 0)
                break;

            if (get_mods() & MOD_BIT(KC_LCTL))
            {
                unregister_code16(KC_LSFT);
                tap_code16(KC_TAB);
                semicolon_nav_activated = 3;
                return false;
            }
            else
                semicolon_nav_activated = 2;
            break;

    }

    return true;
}
















// track the state of CTRL_ESC over multiple key presses.
// 0 - not activated
// 1 - pressed (esc activated, ctrl pending on next keystroke)
// 2 - pressed (ctrl activated, esc pending on CTRL_ESC release if no combo consumption)
// 3 - consumed (upgraded to ctrl or autoexited on special case keys)
static int ctrl_escape_activated;

// A ctrl-esc kétféleképpen aktiválódik attól függően, hogy mennyi idő telt el az utolsó billentyűleütés óta.
// A feltételezés az, hogy ha egy másik billentyű után rövid időn belül nyomjuk meg,
// akkor valószínűleg az Escape funkciót szeretnénk használni
// (pl. Vim beszúrási módjából való kilépéshez), viszont ha csak úgy, előzmény nélkül nyomjuk meg,
// akkor valószínűbb, hogy Ctrl billentyűként szeretnénk használni.
//
// Ha nem történt nemrég billentyűleütés, akkor Ctrl módban indul,
// és csak a felengedéskor küld Escape leütést (ha közben nem használjuk Ctrl-ként).
// Ez lehetővé teszi a Ctrl-kombinációk használatát anélkül, hogy feleslegesen Escape
// billentyűleütés is történne.
//
// Ha egy korábbi billentyűleütés után nyomjuk meg, akkor Escape módban indul,
// és csak akkor vált át Ctrl működésre, ha a lenyomva tartás közben egy másik
// billentyűt is megnyomunk.

// ctrl-esc has two modes of activation depending on how long since the last key press.
// the assumption is that if it is pressed soon after another key, it is probably intended to be escape
// (ie. for vim insert mode exit), but if pressed out of the blue it's more likely to be a ctrl key.
//
// if pressed with no recent key presses, it will begin in ctrl mode, and only actuate an escape press
// on release (if not consumed as control). this allows for ctrl combos without an unnecessary escape fire.
//
// if pressed after a recent key, it will begin in esc mode, and only actuate a control press if another
// key is pressed during the hold.
bool process_ctrl_esc(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CTRL_ESC:
            if (record->event.pressed) {
                // Tehát a CTRL_ESC megnyomásakor PPY nem azonnal dönti el, hogy Ctrl vagy Esc.
                // Ha az előző billentyűzet-esemény óta 350 ms-nál kevesebb telt el:
                if (timer_elapsed(last_key_time) < 350)
                {
                    register_code(KC_ESC);        // tényleges key-down eseményt küld
                    ctrl_escape_activated = 1;
                }
                // Ha 350 ms vagy több telt el:
                else
                {
                    register_code(KC_LCTL);      // tényleges key-down eseményt küld
                    ctrl_escape_activated = 2;
                }
            }
            else
            {
                unregister_code(KC_ESC);         // key-up esemény
                unregister_code(KC_LCTL);        // key-up esemény

                if (ctrl_escape_activated == 2)
                {
                    tap_code16(KC_ESC);
                }

                if (ctrl_escape_activated < 3)
                {
//                     if (IS_LAYER_ON(_VINSERT))
//                         layer_move(_VIM);
                }

                ctrl_escape_activated = 0;
            }

            return true;
        case KC_J:
        case KC_K:
            // these are keys we never expect to be ctrl combos, so remove the ctrl down state
            // to avoid misfires from releasing esc too late.
            if (ctrl_escape_activated == 2)
            {
                unregister_code(KC_LCTL);
                ctrl_escape_activated = 3;
            }
            break;
        default:
            switch (ctrl_escape_activated)
            {
                case 1:
                    // ctrl combo capture.
                    unregister_code(KC_ESC);
                    register_code(KC_LCTL);
                    ctrl_escape_activated = 3;
                    break;
                case 2:
                    // consumption.
                    ctrl_escape_activated = 3;
                    break;
            }
            break;
    }

    return true;
}









bool process_record_user(uint16_t keycode, keyrecord_t *record) {


    if (!process_nav_scln(keycode, record)) return false;

    // A process_ctrl_esc() saját maga kezeli a CTRL_ESC eseményt, és true-t ad vissza.
    // Ha a process_ctrl_esc() azt mondja, hogy ezt az eseményt ne engedjük tovább, akkor álljunk meg itt.
    if (!process_ctrl_esc(keycode, record)) return false;

    // Ez a last_key_time frissítése.
    // A CTRL_ESC működése ugyanis erre támaszkodik:
    // Normál billentyű lenyomásakor frissíti az időt;
    // CTRL_ESC lenyomásakor nem frissíti;
    // CTRL_ESC felengedésekor viszont frissíti.
    // Ez az a rész, ami lehetővé teszi a PPY-nél a CTRL_ESC és a NAV_SCLN közötti speciális gyors kombinációt, illetve a dupla CTRL_ESC viselkedést.
    // in the case of CTRL_ESC, don't update on pressed to allow for CTRL_ESC+SLCN_NAV combo.
    // updating on the release pass allows for double tap on CTRL_ESC to get an ESC hold.
    if (record->event.pressed != (keycode == CTRL_ESC))
        last_key_time = timer_read();

    return true;
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
// record->event.pressed     // true = lenyomás, false = felengedés.
//
// ============================================================================


