bool rgb_matrix_indicators_user(void) {

    // Ha nincs aktív OS-visszajelzés, nincs teendő.
    if (!os_feedback_active) {
        return false;
    }

    // Meghatározzuk, hány ms telt el az OS-visszajelzés kezdete óta.
    uint32_t elapsed = timer_elapsed32(os_feedback_start);


    // ─────────────────────────────────────────────
    // 0–100 ms: nincs visszajelzés
    // ─────────────────────────────────────────────

    // Az első 100 ms-ban még nem jelenítünk meg színt.
    if (elapsed < 100) {
        return false;
    }


    // ─────────────────────────────────────────────
    // 100–5000 ms: OS mód visszajelzése
    // ─────────────────────────────────────────────

    if (elapsed < 5000) {

        // Mac mód = kék
        if (mac_mode) {
            for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
                rgb_matrix_set_color(i, 0, 0, 255);
            }
        }

        // PC mód = piros
        else {
            for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
                rgb_matrix_set_color(i, 255, 0, 0);
            }
        }

        return false;
    }


    // ─────────────────────────────────────────────
    // 5000 ms után: OS-visszajelzés befejezése
    // ─────────────────────────────────────────────

    os_feedback_active = false;

    return false;
}
