// 0 = nincs aktív NAV_SCLN
// 1 = NAV_SCLN aktív, még nincs NAV művelet; 2 = NAV művelet történt; 3 = SHIFT-es speciális állapot
static int semicolon_nav_activated;

bool process_nav_scln(uint16_t keycode, keyrecord_t *record) {

    switch (keycode) {

        case NAV_SCLN:

            // NAV_SCLN lenyomása.
            if (record->event.pressed) {

                // Gyors használat esetén azonnal 'é' karaktert küldünk.
                if (get_mods() & MOD_BIT(KC_LSFT) ||
                    timer_elapsed(last_key_time) < 300)
                {
                    register_code16(KC_SCLN);
                    semicolon_nav_activated = 2;

                    return true;
                }

                // Várunk a következő billentyűre: karakter vagy NAV művelet.
                semicolon_nav_activated = 1;
                nav_scln_time = timer_read();

                layer_on(_NAV);

                return false;
            }

            // NAV_SCLN felengedése.
            else {

                switch (semicolon_nav_activated) {

                    case 1:

                        // Rövid nyomás esetén önálló 'é' karaktert küldünk.
                        if (timer_elapsed(nav_scln_time) < 600)
                        {
                            clear_mods();
                            tap_code16(KC_SCLN);
                        }

                        break;


                    case 2:

                        // Az előzőleg lenyomott 'é' karakter felengedése.
                        unregister_code16(KC_SCLN);

                        break;


                    case 3:

                        // A speciális navigációhoz lenyomott SHIFT felengedése.
                        unregister_code16(KC_LSFT);

                        break;
                }

                semicolon_nav_activated = 0;
                layer_off(_NAV);

                return false;
            }


        // NAV_SCLN után ezek NAV műveletnek számítanak.
        case KC_UP:
        case KC_DOWN:
        case KC_ENT:
        case KC_PGUP:
        case KC_PGDN:
        case KC_LSFT:
        case KC_RSFT:
        case WORD_L:
        case WORD_R:
        case LINE_L:
        case LINE_R:
        case KC_LCTL:
        case TAB_L:
        case TAB_R:
        case CLOSE_W:
        case KC_GRV:

            // Az első NAV billentyű aktiválja a NAV állapotot.
            if (semicolon_nav_activated == 1)
            {
                semicolon_nav_activated = 2;
                return true;
            }

            // Szándékosan nincs break: a feldolgozás továbbesik a CTRL_ESC ágra.


        case CTRL_ESC:

            // Gyors NAV_SCLN + CTRL_ESC esetén először 'é'-t küldünk.
            if (semicolon_nav_activated == 1 &&
                timer_elapsed(last_key_time) < 250)
            {
                tap_code16(KC_SCLN);
                semicolon_nav_activated = 2;
            }

            break;


        default:

            // NAV_SCLN + normál karakter esetén először elküldjük az 'é'-t.
            if (semicolon_nav_activated == 1)
            {
                tap_code16(KC_SCLN);
                semicolon_nav_activated = 2;
            }

            break;


        case KC_LEFT:

            // Release eseménynél vagy inaktív NAV állapotban nincs külön kezelés.
            if (!record->event.pressed ||
                semicolon_nav_activated == 0)
                break;

            // CTRL + LEFT esetén SHIFT + TAB műveletet hajtunk végre.
            if (get_mods() & MOD_BIT(KC_LCTL))
            {
                register_code16(KC_LSFT);
                tap_code16(KC_TAB);

                semicolon_nav_activated = 3;

                return false;
            }
            else
            {
                semicolon_nav_activated = 2;
            }

            break;


        case KC_RGHT:

            // Release eseménynél vagy inaktív NAV állapotban nincs külön kezelés.
            if (!record->event.pressed ||
                semicolon_nav_activated == 0)
                break;

            // CTRL + RIGHT esetén speciális SHIFT + TAB műveletet hajtunk végre.
            if (get_mods() & MOD_BIT(KC_LCTL))
            {
                unregister_code16(KC_LSFT);
                tap_code16(KC_TAB);

                semicolon_nav_activated = 3;

                return false;
            }
            else
            {
                semicolon_nav_activated = 2;
            }

            break;
    }

    return true;
}
