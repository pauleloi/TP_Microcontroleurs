#include "configbits.h"  // Configuration du microcontrôleur
#include <xc.h>          // Bibliothèque XC8

void delai_approx(void) {
    for (unsigned long i = 0; i < 200000; i++) {
        NOP();  // Instruction No Operation pour ralentir
    }
}

void main(void) {
    // Configuration des ports en sortie
    TRISD = 0x00;  // Configure le port D (LEDs D1-D4) en sortie
    TRISB = 0x00;  // Configure le port B (LEDs D5-D8) en sortie

    // Initialisation des ports
    LATD = 0x00;  // Éteint les LEDs D1-D4
    LATB = 0x00;  // Éteint les LEDs D5-D8

    while (1) {
        LATD = 0x0F;  // Allume LEDs D1-D4 (binaire 00001111)
        LATB = 0x00;  // Éteint LEDs D5-D8
        delai_approx();

        LATD = 0x00;  // Éteint LEDs D1-D4
        LATB = 0x0F;  // Allume LEDs D5-D8 (binaire 00001111)
        delai_approx();
    }
}