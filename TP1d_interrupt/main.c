#include "configbits.h"
#include <xc.h>
#include <stdbool.h>  // Pour gérer echange en booléen

#define LEDS_D LATD
#define LEDS_B LATB
#define ALL_OFF 0x00
#define FIRST_LED 0x01
#define LAST_LED  0x10

volatile int echange = 0;  // Variable modifiée dans l'interruption
volatile int cpt = 0;
volatile unsigned char chenillard = FIRST_LED;  // LED active

// Initialisation des ports
void init_ports(void) {
    TRISD = 0x00;  // Port D en sortie pour LEDs D1-D4
    TRISB = 0x00;  // Port B en sortie pour LEDs D5-D8
    LEDS_D = ALL_OFF;  // Éteindre LEDs D1-D4
    LEDS_B = ALL_OFF;  // Éteindre LEDs D5-D8
}

// Initialisation du Timer2 avec interruption
void init_timer2(void) {
    PR2 = 124;               // Valeur de PR2 pour obtenir 1ms
    T2CONbits.TMR2ON = 1;    // Activer Timer2
    T2CONbits.T2CKPS = 0b00; // Prescaler = 1:1
    T2CONbits.T2OUTPS = 0b1111; // Postscaler = 1:16

    PIE1bits.TMR2IE = 1; // Activation de l'interruption Timer2
    INTCONbits.PEIE = 1; // Activation des interruptions périphériques
    INTCONbits.GIE = 1;  // Activation des interruptions globales
}

// Routine d'interruption (ISR)
void __interrupt() isr(void) {
    if (PIR1bits.TMR2IF) {  // Vérifier si l'interruption provient du Timer2
        PIR1bits.TMR2IF = 0;  // Réinitialiser le flag

        cpt++; // Incrémentation du compteur AVANT la vérification

        if (cpt >= 125) { // Exécuter le chenillard une fois tous les 125 cycles
            cpt = 0;      // Réinitialiser le compteur

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
        }

    }
}

void main(void) {
    init_ports();
    init_timer2();

    while (1) {
        // Le programme principal ne fait rien
        // Tout est géré par l'interruption Timer 2
    }
}