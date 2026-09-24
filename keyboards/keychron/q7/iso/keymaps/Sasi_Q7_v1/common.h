
enum q7_layers{
    _BASE,
    _NAV,
};

enum custom_keycodes {
    OS_TOGGLE,
    CTRL_ESC,
    NAV_SCLN,
    WORD_L,
    WORD_R,
    LINE_L,
    LINE_R,
    TAB_L,
    TAB_R,
    CLOSE_W,
};

#define WORD_L LALT(KC_LEFT)
#define WORD_R LALT(KC_RIGHT)

#define LINE_L LGUI(KC_LBRC)
#define LINE_R LGUI(KC_RBRC)

#define CUT   LGUI(KC_X)
#define COPY  LGUI(KC_C)
#define PASTE LGUI(KC_V)

// ezek még nincsennek megvalósítva
//enum planck_keycodes {
//    QWERTY = SAFE_RANGE,
//    BACKLIT,
//    KVM_SWT,
//    COPYADDR,
//    SYMBOL,
//    GAME,
//    MD_CODE,
//    MEH_ENT,
//    LOCK,
//    S1,
//    S2,
//    S3,
//    S4,
//    S5,
//    S6,
//};

#define OS_MODE_MAC 0x01

