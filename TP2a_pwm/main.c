#include "configbits.h"
#include <xc.h>

#define _XTAL_FREQ 8000000  // Fréquence du microcontrôleur

// Définition des ports et boutons
#define LED_PORT  RD0PPS  // Attribution PPS pour PWM4

#define BOUTON_TRIS  TRISB0         // Bouton sur RB0
#define BOUTON_PORT  PORTBbits.RB0  // Lecture du bouton

// Paramètres du PWM
#define PWM_PRESCALER 0b10  // Prescaler 16
#define PWM_POSTSCALER 0b0000  // Postscaler 1:1
#define PWM_PERIOD 199  // PR2 = 199 pour obtenir 1 kHz

// Définition des duty cycles
#define DUTY_CYCLE_LOW  0.1  // 10% lorsque le bouton est relâché
#define DUTY_CYCLE_HIGH 1.0  // 100% lorsque le bouton est appuyé

// Initialisation des ports et du bouton
void init_ports(void) {
    // Configuration des sorties pour les LEDs
    TRISD = 0x00;  // PORTD en sortie
    LATD = 0x00;   // Éteindre LEDs sur PORTD

    // Configuration du bouton en entrée
    TRISBbits.BOUTON_TRIS = 1; // PORTB0 en entrée
    ANSELB = 0x00;             // Désactiver les entrées analogiques (car utilisation du bouton)
}

// Initialisation de la PWM4
void init_pwm4(void) {
    // Configurer Timer 2 pour la PWM
    T2CONbits.T2CKPS = PWM_PRESCALER;    // Prescaler défini
    T2CONbits.T2OUTPS = PWM_POSTSCALER;  // Postscaler défini
    PR2 = PWM_PERIOD;                    // Période de la PWM

    // Activer de PWM4
    PWM4CONbits.PWM4EN = 1;

    // Attribution PPS pour RD0 (Sortie PWM)
    LED_PORT = 0x0F;

    // Démarrer Timer 2
    T2CONbits.TMR2ON = 1;
}

// Fonction pour ajuster le duty cycle du PWM
void set_pwm_duty_cycle(float duty_cycle_ratio) {
    // (PWMxDCH:PWMxDCL<7:6>)=DutyCycleRatio×4×(PR2+1)
    unsigned int pwm_value = (unsigned int)(duty_cycle_ratio * 4.0 * (PR2 + 1));  // Equation 17-3 page 179

    PWM4DCH = (pwm_value >> 2) & 0xFF;  // Stockage des 8 bits de poids fort
    PWM4DCL = (pwm_value << 6) & 0xC0;  // Stockage des 2 bits de poids faible
}

// Mise à jour du PWM selon l'état du bouton
void update_pwm_duty_cycle(void) {
    // if
    set_pwm_duty_cycle((BOUTON_PORT == 0) ? DUTY_CYCLE_HIGH : DUTY_CYCLE_LOW);
}

// Fonction de configuration initiale
void setup(void) {
    init_ports();
    init_pwm4();
}

// Boucle principale
void main(void) {
    setup();

    while (1) {
        update_pwm_duty_cycle();  // Ajuster la luminosité de la LED en fonction du bouton
    }
}
