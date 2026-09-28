void matrix_scan_user(void) {
    if (!os_feedback_active) {
        return;
    }

    uint32_t elapsed = timer_elapsed32(os_feedback_start);

    // 0–100 ms: minden LED ki
    if (elapsed < 100) {
        return;
    }

    // 100–500 ms: OS színének megjelenítése
    if (elapsed < 500) {
        rgb_matrix_enable_noeeprom();

        // Mac = kék
        // PC  = piros
        if (mac_mode) {
            rgb_matrix_sethsv_noeeprom(170, 255, 255);
        } else {
            rgb_matrix_sethsv_noeeprom(0, 255, 255);
        }

        rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);

        return;
    }

    // 500 ms után az eredeti RGB állapot visszaállítása
    rgb_matrix_enable_noeeprom();

    rgb_matrix_sethsv_noeeprom(
        saved_rgb_h,
        saved_rgb_s,
        saved_rgb_v
    );

    rgb_matrix_set_speed_noeeprom(saved_rgb_speed);
    rgb_matrix_mode_noeeprom(saved_rgb_mode);

    os_feedback_active = false;
}
