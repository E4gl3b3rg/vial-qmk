void unregister_nav_scln_down_state(void) {
    unregister_code16(KC_SCLN);

    switch (semicolon_nav_activated)
    {
        case 3:
            unregister_code16(KC_LSFT);
            break;
        case 4:
            unregister_code16(KC_LGUI);
            break;
    }

    semicolon_nav_activated = 0;
}
