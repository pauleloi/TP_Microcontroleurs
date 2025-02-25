#include "configbits.h"
#include <xc.h>

#define LEDS_D LATD
#define LEDS_B LATB
#define ALL_OFF 0x00
#define FIRST_LED 0x01
#define LAST_LED  0x10

// Initialisation des ports
void init_ports(void) {
    TRISD = 0x00;  // Port D en sortie pour LEDs D1-D4
    TRISB = 0x00;  // Port B en sortie pour LEDs D5-D8
    LEDS_D = ALL_OFF;  // Éteindre LEDs D1-D4
    LEDS_B = ALL_OFF;  // Éteindre LEDs D5-D8
}

// Initialisation du Timer2 avec valeurs en brut
void init_timer2(void) {
    PR2 = 124;            // Valeur brute pour PR2 (1 ms)
    T2CONbits.TMR2ON = 1; // Activer le Timer2
    T2CONbits.T2CKPS = 0b00;  // Prescaler = 1:1
    T2CONbits.T2OUTPS = 0b1111; // Postscaler = 1:16
}

void main(void) {
    init_ports();
    init_timer2();

    int compteur = 0;
    int echange = 0;  // Permet de basculer entre PORTD et PORTB
    unsigned char chenillard = FIRST_LED;  // LED actuelle

    while (1) {
        if (echange == 0) {
            LEDS_B = ALL_OFF;     // Éteindre LEDs de PORTB
            LEDS_D = chenillard;  // Allumer LED de PORTD
            chenillard <<= 1;     // Décaler la LED vers la gauche
            if (chenillard == LAST_LED) {  // Si on arrive à la dernière LED
                echange = 1;               // On passe à PORTB
                chenillard = FIRST_LED;    // On recommence depuis la première LED
            }
        } else {
            LEDS_D = ALL_OFF;     // Éteindre LEDs de PORTD
            LEDS_B = chenillard;  // Allumer LEDs de PORTB
            chenillard <<= 1;     // Décaler la LED vers la gauche
            if (chenillard == LAST_LED) {  // Si on arrive à la dernière LED
                echange = 0;               // On repasse à PORTD
                chenillard = FIRST_LED;    // On recommence depuis la première LED
            }
        }

        // Attendre 125 interruptions du Timer2 (1ms * 125 = 125ms)
        while (compteur < 125) {
            while (!PIR1bits.TMR2IF) {}  // Attendre que le Timer2 expire
            PIR1bits.TMR2IF = 0;         // Réinitialiser le flag
            compteur++;
        }
        compteur = 0;
    }
}