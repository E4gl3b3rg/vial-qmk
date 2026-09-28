// Mac / PC mód kezelése.
// Az OS_TOGGLE billentyű megnyomásakor átváltunk a Mac és a PC mód között.
// A kiválasztott módot az EEPROM-ba is elmentjük, így a billentyűzet
// újraindítása vagy áramtalanítása után is megmarad a beállítás.
bool process_os_toggle(uint16_t keycode, keyrecord_t *record) {
    if (keycode != OS_TOGGLE) {
        return true;
    }

    // ez a felengedés, itt már nem csinálunk semmit.
    if (!record->event.pressed) {
        return false;
    }

    mac_mode = !mac_mode;

    uprintf("OS TOGGLE -> mac_mode=%d\n", mac_mode);

    eeconfig_update_user(mac_mode ? OS_MODE_MAC : 0);

    // RGB visszajelzés indítása
    saved_rgb_mode  = rgb_matrix_get_mode();
    saved_rgb_h     = rgb_matrix_get_hue();
    saved_rgb_s     = rgb_matrix_get_sat();
    saved_rgb_v     = rgb_matrix_get_val();
    saved_rgb_speed = rgb_matrix_get_speed();

    os_feedback_active = true;
    os_feedback_start = timer_read32();

    rgb_matrix_disable_noeeprom();

    return false;
}
