bool process_word_keys(uint16_t keycode, keyrecord_t *record) {

    switch (keycode) {

        case WORD_L:
        uprintf("WORD_L\n");
            if (record->event.pressed) {
                if (mac_mode) {
                    tap_code16(LGUI(KC_LEFT));
                } else {
                    tap_code16(LCTL(KC_LEFT));
                }
            }
            return false;

        case WORD_R:
        uprintf("WORD_R\n");
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
