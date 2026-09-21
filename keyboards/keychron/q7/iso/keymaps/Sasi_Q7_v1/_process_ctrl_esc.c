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
                // okay this is getting a bit silly so you might ask "why not just split out into separate keys"
                // well i'm not ready to do that yet, and the remaining complexity i'm adding here is to handle
                // very edge cases.

                // only immediate escape if the last keypress was quite recent..
                if (timer_elapsed(last_key_time) < 200
                        // ..and no modifiers are held..
                        && get_mods() == 0
                        // last key wasn't space (ie. `vi <C-t>` in terminal)
                        && last_key_code != KC_SPC
                        // ..and either the last key was CTRL_ESC (two pressed in fast succession)..
                        && (last_key_code == CTRL_ESC
                        // ..or we are pretty sure that we're performing a vim normal move return.
                        // - we haven't been 'jk'-ing for too long in a row
                        // - we pressed 'i' more recently than 'esc'
                        || (vim_movement < 2 && vim_insert))
                   )
                {
                    register_code(KC_ESC);
                    ctrl_escape_activated = 1;
                }
                else
                {
                    register_code(KC_LCTL);
                    ctrl_escape_activated = 2;
                }
            }
            else
            {
                unregister_code(KC_ESC);
                unregister_code(KC_LCTL);

                if (timer_elapsed(last_key_time) < 250 && ctrl_escape_activated == 2)
                {
                    tap_code16(KC_ESC);
                }

                ctrl_escape_activated = 0;
            }

            return true;
        default:
            if (record->event.pressed) {
                switch (ctrl_escape_activated)
                {
                    case 1:
                        // ctrl combo capture.
                        unregister_code(KC_ESC);
                        // register_code(KC_LCTL);
                        ctrl_escape_activated = 3;
                        break;
                    case 2:
                        // consumption.
                        ctrl_escape_activated = 3;
                        break;
                }
            }
            break;
    }

    return true;
}
