#include "configbits.h"
#include <xc.h>

#define _XTAL_FREQ 8000000  // Fréquence du microcontrôleur

// Définition des ports
#define LED_PORT  RD0PPS  // Attribution PPS pour PWM4

// Variables globales pour l?interruption
volatile float duty_cycle = 0.0;
volatile int step = 1;  // 1 pour montée, -1 pour descente
volatile int count = 0;

// Initialisation des ports
void init_ports(void) {
    TRISD = 0x00;  // Port D en sortie pour LEDs D1-D4
    TRISB = 0x00;  // Port B en sortie pour LEDs D5-D8
    LATD = 0x00;  // Éteindre LEDs D1-D4
    LATD = 0x00;  // Éteindre LEDs D5-D8
}

// Initialisation de la PWM4
void init_pwm4(void) {
    // Configurer Timer 2 pour la PWM
    PR2 = 124;               // Valeur de PR2 pour obtenir 1ms
    T2CONbits.TMR2ON = 1;    // Activer Timer2
    T2CONbits.T2CKPS = 0b00; // Prescaler = 1:1
    T2CONbits.T2OUTPS = 0b1111; // Postscaler = 1:16

    // Activer PWM4
    PWM4CONbits.PWM4EN = 1;

    // Attribution PPS pour RD0 (Sortie PWM)
    LED_PORT = 0x0F;

    // Démarrer Timer 2
    T2CONbits.TMR2ON = 1;
}

// Configuration de l?interruption du Timer2
void init_timer2_interrupt(void) {
    // Activer l?interruption du Timer2
    PIE1bits.TMR2IE = 1;  // Interruption Timer2 activée
    INTCONbits.PEIE = 1;  // Activer les interruptions périphériques
    INTCONbits.GIE = 1;   // Activer les interruptions globales
}

// Fonction pour ajuster le duty cycle du PWM
void set_pwm_duty_cycle(float duty_cycle_ratio) {
    // (PWMxDCH:PWMxDCL<7:6>) = Duty Cycle Ratio × 4 × (PR2 + 1)
    unsigned int pwm_value = (unsigned int)(duty_cycle_ratio * 4.0 * (PR2 + 1));

    PWM4DCH = (pwm_value >> 2) & 0xFF;  // 8 bits de poids fort
    PWM4DCL = (pwm_value << 6) & 0xC0;  // 2 bits de poids faible
}

// Routine d?interruption du Timer2
void __interrupt() isr(void) {
    if (PIR1bits.TMR2IF) {  // Vérifier si c?est une interruption du Timer2
        PIR1bits.TMR2IF = 0;  // Réinitialiser le flag d?interruption

        //Update toutes les 10ms
        count++;

        if (count >= 10) {  // Modifier le duty cycle toutes les 10 interruptions (~100 ms)
            count = 0;  // Réinitialiser le compteur

            duty_cycle += 0.01 * step;  // Augmentation/diminution en 100 étapes

            // Inversion du sens de variation aux bornes
            if (duty_cycle >= 1.0) {
                duty_cycle = 1.0;
                step = -1;  // Passer à la diminution
            } else if (duty_cycle <= 0.0) {
                duty_cycle = 0.0;
                step = 1;  // Passer à l'augmentation
            }

            // Appliquer la nouvelle valeur du duty cycle
            set_pwm_duty_cycle(duty_cycle);
        }
    }
}

// Fonction de configuration initiale
void setup(void) {
    init_ports();
    init_pwm4();
    init_timer2_interrupt();
}

// Boucle principale
void main(void) {
    setup();

    while (1) {
        // La mise à jour du duty cycle est maintenant gérée par l?interruption
    }
}