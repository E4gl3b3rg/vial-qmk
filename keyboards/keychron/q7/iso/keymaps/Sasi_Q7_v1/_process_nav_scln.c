bool process_nav_scln(uint16_t keycode, keyrecord_t *record) {
    // handle toggle logic (and potential semicolon output).
    switch (keycode) {
        case NAV_SCLN:
            if (record->event.pressed) {
                // timer case here is just to give more immediacy to semicolons when typing at EOL.
                // set low enough to not impede intended navigation
                if (get_mods() & MOD_BIT(KC_LSFT) || (get_mods() == 0 && timer_elapsed(last_key_time) < 100 && last_key_code != CTRL_ESC))
                {
                    register_code16(KC_SCLN);
                    semicolon_nav_activated = 2;
                    return true;
                }
                else
                {
                    semicolon_nav_activated = 1;
                    layer_on(_NAV);
                }
                return false;
            }
            else
            {
                switch (semicolon_nav_activated)
                {
                    case 1:
                        if (timer_elapsed(last_key_time) < 250)
                        {
                            // clears a potential ctrl modifier from CTRL_ESC.
                            // sequence of events is SCLN down - wait 500ms - CTRL_ESC down - SCLN up - CTRL_ESC up
                            clear_mods();
                            tap_code16(KC_SCLN);
                        }
                        break;
                }

                unregister_nav_scln_down_state();
                semicolon_nav_activated = 0;
                layer_off(_NAV);
                return false;
            }
    }


    if (!record->event.pressed || semicolon_nav_activated < 1)
        return true;

    switch (keycode) {
        case KC_UP:
        case KC_DOWN:
        case KC_PGUP:
        case KC_PGDN:
        case WORD_L:
        case WORD_R:
        case LINE_L:
        case LINE_R:
        case TAB_L:
        case TAB_R:
            unregister_nav_scln_down_state();
            semicolon_nav_activated = 2;

            // actions for these are handled by the keys/macros temselves.
            return true;

        case KC_LEFT:
        case KC_RGHT:
            if (get_mods() & MOD_BIT(KC_LCTL) || get_mods() & MOD_BIT(KC_LGUI))
            {
                if (semicolon_nav_activated != 3)
                    unregister_nav_scln_down_state();

                if (keycode == KC_LEFT)
                    register_code16(KC_LSFT);
                else
                    unregister_code16(KC_LSFT);

                tap_code16(KC_TAB);
                semicolon_nav_activated = 3;
                // don't fire escape after usage. has to be handled here due to the blocking return.
                ctrl_escape_activated = 3;
                return false;
            }
            else
            {
                unregister_nav_scln_down_state();
                semicolon_nav_activated = 2;
                return true;
            }

        case KC_LGUI:
        case KC_LSFT:
            return true;

        case KC_ENT:
            // enter only cmd enters if the first thing.
            // this is to allow things like selecting options from lists (using up/down).
            if (semicolon_nav_activated > 1)
                return true;

            break;

        case CTRL_ESC:
        case KC_SPC:
            // this handles cases like SCLN_NAV -> KC_ESC rapidly after a previous character.
            if (semicolon_nav_activated == 1 && timer_elapsed(last_key_time) < 500)
            {
                tap_code16(KC_SCLN);
                semicolon_nav_activated = 2;
                return true;
            }

            break;
    }

    // if the nav layer doesn't have a specific action for the pressed key, we add a cmd modifier.
    // this feels pretty good but may be a bit noisy/undefined.
    if (semicolon_nav_activated != 4 && semicolon_nav_activated != 2)
    {
        unregister_nav_scln_down_state();

        register_code16(KC_LGUI);
        semicolon_nav_activated = 4;
    }

    return true;
}
