bool process_word_keys(uint16_t keycode, keyrecord_t *record) {
    uprintf("process_word_keys meghivva\n");

    uprintf("keycode=%d\n", keycode);
    switch (keycode) {

        case CUT:
            if (record->event.pressed) {
                if (mac_mode) {
                    tap_code16(LALT(KC_X));
                } else {
                    tap_code16(LCTL(KC_X));
                }
            }
            return false;

        case COPY:
            if (record->event.pressed) {
                if (mac_mode) {
                    tap_code16(LALT(KC_C));
                } else {
                    tap_code16(LCTL(KC_C));
                }
            }
            return false;

        case PASTE:
            if (record->event.pressed) {
                if (mac_mode) {
                    tap_code16(LALT(KC_V));
                } else {
                    tap_code16(LCTL(KC_V));
                }
            }
            return false;

        case WORD_R:
            uprintf("WORD_R\n");

            if (record->event.pressed) {
                if (mac_mode) {
                    tap_code16(LGUI(KC_LEFT));
                } else {
                    tap_code16(LCTL(KC_LEFT));
                }
            }

            return false;

        case WORD_L:
            uprintf("WORD_L\n");

            if (record->event.pressed) {
                if (mac_mode) {
                    tap_code16(LGUI(KC_RIGHT));
                } else {
                    tap_code16(LCTL(KC_RIGHT));
                }
            }

            return false;
    }

    uprintf("return true\n");
    return true;
}
