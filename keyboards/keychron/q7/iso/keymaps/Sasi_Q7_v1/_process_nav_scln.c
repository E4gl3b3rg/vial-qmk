// A NAV_SCLN aktuális állapotát tárolja.
// 0 = nincs aktív NAV_SCLN
// 1 = NAV_SCLN le van nyomva, még nincs NAV művelet
// 2 = NAV mód aktiválódott / az 'é' már kezelve lett
// 3 = speciális SHIFT-es navigációs állapot
static int semicolon_nav_activated;

bool process_nav_scln(uint16_t keycode, keyrecord_t *record) {

    /*
     * ================================================================
     * NAV_SCLN
     * ================================================================
     *
     * A NAV_SCLN egy kettős funkciójú billentyű:
     *
     *   1. önmagában / karakterként:
     *        -> KC_SCLN
     *        -> magyar kiosztáson ez 'é'
     *
     *   2. NAV billentyűként:
     *        -> a _NAV layer aktiválódik
     *        -> kurzormozgatás, szó/lapozás stb.
     *
     * A működéshez a semicolon_nav_activated változó állapotot
     * tartunk fenn.
     *
     * Állapotok:
     *
     *   0 = nincs aktív NAV_SCLN
     *   1 = NAV_SCLN le van nyomva, még nincs NAV művelet
     *   2 = NAV mód aktiválódott / az 'é' már kezelve lett
     *   3 = speciális SHIFT-es navigációs állapot
     *
     * ================================================================
     */


    switch (keycode) {


        /*
         * ============================================================
         * NAV_SCLN
         * ============================================================
         *
         * Ez maga a fizikai 'é' billentyű.
         *
         * Lenyomáskor eldöntjük:
         *
         *   - gyors karakteres használat
         *   - vagy NAV mód
         *
         * Felengedéskor pedig lezárjuk az állapotot.
         */
        case NAV_SCLN:


            /*
             * --------------------------------------------------------
             * NAV_SCLN LENYOMÁSA
             * --------------------------------------------------------
             */
            if (record->event.pressed) {


                /*
                 * ====================================================
                 * GYORS KARAKTERES ESET
                 * ====================================================
                 *
                 * Ha:
                 *
                 *   - SHIFT le van nyomva
                 *
                 * vagy
                 *
                 *   - az előző billentyű óta kevesebb mint 300 ms telt el
                 *
                 * akkor nem lépünk NAV módba.
                 *
                 * Azonnal elküldjük a KC_SCLN kódot.
                 *
                 * Magyar billentyűzetkiosztáson:
                 *
                 *     KC_SCLN -> é
                 *
                 * A register_code16() itt azért kell, mert a kódot
                 * lenyomott állapotban tartjuk.
                 */
                if (get_mods() & MOD_BIT(KC_LSFT) ||
                    timer_elapsed(last_key_time) < 300)
                {
                    register_code16(KC_SCLN);

                    /*
                     * 2-es állapot:
                     * az 'é' karakter már aktív.
                     */
                    semicolon_nav_activated = 2;

                    return true;
                }


                /*
                 * ====================================================
                 * NAV MÓDRA VÁLTÁS
                 * ====================================================
                 *
                 * Ha nem volt gyors karakteres eset, akkor még nem
                 * tudjuk, hogy a felhasználó:
                 *
                 *     'é'-t akar írni
                 *
                 * vagy:
                 *
                 *     NAV_SCLN + valamilyen navigációs billentyűt
                 *
                 * ezért 1-es állapotba kerülünk.
                 */
                semicolon_nav_activated = 1;


                /*
                 * Megjegyezzük a NAV_SCLN pontos lenyomási idejét.
                 *
                 * Ezt később a felengedéskor használjuk annak
                 * eldöntésére, hogy rövid vagy hosszú nyomás történt.
                 */
                nav_scln_time = timer_read();


                /*
                 * Bekapcsoljuk a NAV layert.
                 *
                 * Ettől kezdve a következő billentyűk a _NAV layer
                 * kiosztása szerint működhetnek.
                 */
                layer_on(_NAV);


                /*
                 * A NAV_SCLN önmagában még ne kerüljön tovább a
                 * normál QMK feldolgozásba.
                 */
                return false;
            }


            /*
             * --------------------------------------------------------
             * NAV_SCLN FELENGEDÉSE
             * --------------------------------------------------------
             */
            else
            {

                /*
                 * Az aktuális állapot alapján döntjük el,
                 * mi legyen a felengedés eredménye.
                 */
                switch (semicolon_nav_activated)
                {


                    /*
                     * =================================================
                     * 1-ES ÁLLAPOT
                     * =================================================
                     *
                     * NAV_SCLN le volt nyomva, de közben nem történt
                     * NAV művelet.
                     *
                     * Ez az önálló 'é' használat esete.
                     */
                    case 1:

                        /*
                         * Csak akkor írjuk ki az 'é'-t, ha a billentyűt
                         * legfeljebb 600 ms-ig tartottuk nyomva.
                         *
                         * 600 ms alatt:
                         *
                         *     NAV_SCLN -> é
                         *
                         * 600 ms vagy hosszabb:
                         *
                         *     NAV_SCLN -> semmi
                         *
                         * Ez akadályozza meg, hogy egy hosszú
                         * NAV_SCLN nyomás végén véletlenül 'é'
                         * kerüljön a szövegbe.
                         */
                        if (timer_elapsed(nav_scln_time) < 600)
                        {
                            /*
                             * Esetleges CTRL modifier eltávolítása.
                             */
                            clear_mods();

                            /*
                             * KC_SCLN küldése.
                             *
                             * Magyar kiosztáson ez 'é'.
                             */
                            tap_code16(KC_SCLN);
                        }

                        break;


                    /*
                     * =================================================
                     * 2-ES ÁLLAPOT
                     * =================================================
                     *
                     * Az 'é' már korábban register_code16()-tal
                     * lenyomásra került.
                     *
                     * Most felengedjük.
                     */
                    case 2:

                        unregister_code16(KC_SCLN);

                        break;


                    /*
                     * =================================================
                     * 3-AS ÁLLAPOT
                     * =================================================
                     *
                     * Egy speciális SHIFT-es navigációs állapot.
                     *
                     * Ilyenkor a korábban regisztrált SHIFT-et
                     * engedjük fel.
                     */
                    case 3:

                        unregister_code16(KC_LSFT);

                        break;
                }


                /*
                 * A NAV_SCLN művelet befejeződött.
                 *
                 * Visszaállítjuk az alapállapotot.
                 */
                semicolon_nav_activated = 0;


                /*
                 * Kikapcsoljuk a NAV layert.
                 */
                layer_off(_NAV);


                /*
                 * A NAV_SCLN release eseményt ne dolgozza fel
                 * tovább a QMK.
                 */
                return false;
            }


        /*
         * ============================================================
         * NAV FUNKCIÓK
         * ============================================================
         *
         * Ezek olyan billentyűk, amelyek NAV_SCLN után közvetlenül
         * navigációs funkciót jelentenek.
         */
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


            /*
             * Csak akkor változtatunk állapotot, ha a NAV_SCLN
             * valóban aktív és még 1-es állapotban van.
             */
            if (semicolon_nav_activated == 1)
            {
                /*
                 * A következő billentyű már NAV művelet volt.
                 *
                 * Ezért nincs szükség arra, hogy a NAV_SCLN
                 * felengedésekor 'é'-t írjunk.
                 */
                semicolon_nav_activated = 2;

                return true;
            }


        /*
         * FONTOS:
         *
         * Itt nincs break.
         *
         * Ha a fenti if nem teljesült, a feldolgozás továbbesik
         * a CTRL_ESC esethez.
         *
         * Ez az eredeti működés része, ezért nem változtatjuk meg.
         */


        /*
         * ============================================================
         * CTRL_ESC
         * ============================================================
         */
        case CTRL_ESC:


            /*
             * Ha NAV_SCLN után nagyon gyorsan CTRL_ESC érkezik,
             * akkor először küldünk egy 'é' karaktert.
             *
             * A last_key_time itt NEM a NAV_SCLN lenyomási ideje,
             * mert azt a process_record_user() csak később frissíti.
             *
             * Ezért ez az eredeti időzítési logika.
             */
            if (semicolon_nav_activated == 1 &&
                timer_elapsed(last_key_time) < 250)
            {
                /*
                 * 'é'
                 */
                tap_code16(KC_SCLN);

                /*
                 * Az 'é' karaktert már kezeltük.
                 */
                semicolon_nav_activated = 2;
            }

            break;


        /*
         * ============================================================
         * MINDEN MÁS BILLENTYŰ
         * ============================================================
         *
         * Ez a legfontosabb rész a normál gépelés szempontjából.
         *
         * Például:
         *
         *     NAV_SCLN + S
         *
         * esetén ide jutunk.
         */
        default:


            /*
             * Ha a NAV_SCLN még 1-es állapotban van, akkor a
             * felhasználó nem navigációs billentyűt nyomott.
             *
             * Ez azt jelenti:
             *
             *     NAV_SCLN + S
             *
             * valójában:
             *
             *     é + S
             */
            if (semicolon_nav_activated == 1)
            {

                /*
                 * Először elküldjük az 'é'-t.
                 *
                 * Ez azért történik itt, hogy az 'é' megelőzze
                 * a következő karaktert.
                 */
                tap_code16(KC_SCLN);


                /*
                 * Az 'é' karakteres rész lezárult.
                 */
                semicolon_nav_activated = 2;
            }

            break;


        /*
         * ============================================================
         * BALRA
         * ============================================================
         */
        case KC_LEFT:


            /*
             * Release eseménynél vagy inaktív állapotban nincs
             * külön NAV kezelés.
             */
            if (!record->event.pressed ||
                semicolon_nav_activated == 0)
                break;


            /*
             * CTRL + LEFT esetén:
             *
             *     SHIFT + TAB
             *
             * jellegű speciális navigáció történik.
             */
            if (get_mods() & MOD_BIT(KC_LCTL))
            {
                /*
                 * SHIFT lenyomása.
                 */
                register_code16(KC_LSFT);


                /*
                 * SHIFT + TAB.
                 */
                tap_code16(KC_TAB);


                /*
                 * 3-as állapot:
                 * SHIFT-et nekünk kell majd felengedni.
                 */
                semicolon_nav_activated = 3;


                /*
                 * A KC_LEFT eseményt ne engedjük tovább.
                 */
                return false;
            }
            else
            {
                /*
                 * Normál NAV balra.
                 */
                semicolon_nav_activated = 2;
            }

            break;


        /*
         * ============================================================
         * JOBBRA
         * ============================================================
         */
        case KC_RGHT:


            /*
             * Release eseménynél vagy inaktív állapotban nincs
             * külön NAV kezelés.
             */
            if (!record->event.pressed ||
                semicolon_nav_activated == 0)
                break;


            /*
             * CTRL + RIGHT esetén speciális SHIFT + TAB kezelés.
             */
            if (get_mods() & MOD_BIT(KC_LCTL))
            {
                /*
                 * A SHIFT felengedése.
                 */
                unregister_code16(KC_LSFT);


                /*
                 * TAB küldése.
                 */
                tap_code16(KC_TAB);


                /*
                 * Speciális SHIFT-es állapot.
                 */
                semicolon_nav_activated = 3;


                /*
                 * Az eredeti RIGHT eseményt ne engedjük tovább.
                 */
                return false;
            }
            else
            {
                /*
                 * Normál NAV jobbra.
                 */
                semicolon_nav_activated = 2;
            }

            break;
    }


    /*
     * Ha egyik speciális ág sem kérte az esemény blokkolását,
     * akkor a billentyű normálisan továbbmehet.
     */
    return true;
}

/*
A kódban most három különböző időlogika van, ezeket érdemes fejben különválasztani:

300 ms
│
├── NAV_SCLN lenyomásakor
│   gyors előző billentyű után → azonnali é
│
250 ms
│
├── NAV_SCLN + CTRL_ESC speciális eset
│
600 ms
│
└── NAV_SCLN önmagában tartva
    600 ms alatt → é
    600 ms vagy tovább → semmi



És a legfontosabb állapotátmenet normál gépelésnél:

NAV_SCLN ↓
    │
    ▼
activated = 1
    │
    │  _NAV layer bekapcsol
    │
    ▼
következő billentyű
    │
    ├── NAV billentyű ──→ activated = 2 ──→ navigáció
    │
    └── normál karakter
             │
             ▼
         KC_SCLN → é
             │
             ▼
         activated = 2
             │
             ▼
       normál karakter
*/
