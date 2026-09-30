// Mac / PC mód kezelése.
//
// Az OS_TOGGLE billentyű lenyomásakor átváltunk a Mac és a PC mód között.
// A kiválasztott módot az EEPROM-ba is elmentjük.
bool process_os_toggle(uint16_t keycode, keyrecord_t *record) {

    // Nem az OS_TOGGLE billentyű eseménye.
    if (keycode != OS_TOGGLE) {
        return true;
    }

    // A felengedést nem dolgozzuk fel.
    if (!record->event.pressed) {
        return false;
    }

    // Mac ↔ PC mód váltása.
    mac_mode = !mac_mode;

    uprintf("OS TOGGLE -> mac_mode=%d\n", mac_mode);

    // OS mód mentése EEPROM-ba.
    eeconfig_update_user(mac_mode ? OS_MODE_MAC : 0);


    // ─────────────────────────────────────────────
    // Jelenlegi RGB állapot mentése
    // ─────────────────────────────────────────────

    saved_rgb_mode  = rgb_matrix_get_mode();
    saved_rgb_h     = rgb_matrix_get_hue();
    saved_rgb_s     = rgb_matrix_get_sat();
    saved_rgb_v     = rgb_matrix_get_val();
    saved_rgb_speed = rgb_matrix_get_speed();


    // Elindítjuk az OS visszajelzést.
    os_feedback_active = true;

    // Eltároljuk a kezdési időpontot.
    os_feedback_start = timer_read32();

    return false;
}
