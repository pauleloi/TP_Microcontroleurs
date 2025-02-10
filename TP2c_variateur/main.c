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
#define DUTY_CYCLE_LOW  0.0  // 10% lorsque le bouton est relâché
#define DUTY_CYCLE_HIGH 1.0  // 100% lorsque le bouton est appuyé

// Initialisation des ports et du bouton
void init_ports(void) {
    // Configuration des sorties pour les LEDs
    TRISD = 0x00;  // PORTD en sortie
    LATD = 0x00;   // Éteindre LEDs sur PORTD
    TRISB = 0x00;  // PORTB en sortie
    LATB = 0x00;   // Éteindre LEDs sur PORTB

    // Configuration du potentiomètre en entrée analogique
    TRISA |= 0x01;  // Active uniquement le bit 0 de TRISA (RA0 en entrée)
    ANSELA |= 0x01; // Active uniquement le bit 0 de ANSELA (RA0 en mode analogique)
}

// Initialisation de la PWM4
void init_pwm4(void) {
    // Configurer Timer 2 pour la PWM
    T2CONbits.T2CKPS = PWM_PRESCALER;    // Prescaler défini
    T2CONbits.T2OUTPS = PWM_POSTSCALER;  // Postscaler défini
    PR2 = PWM_PERIOD;                    // Période de la PWM

    // Activater de PWM4
    PWM4CONbits.PWM4EN = 1;

    // Attribution PPS pour RD0 (Sortie PWM)
    LED_PORT = 0x0F;

    // Démarrer Timer 2
    T2CONbits.TMR2ON = 1;
}

// Initialisation de l'ADC
void init_adc(void) {
    ADCON0 = (0 << 2);    // Sélectionner le canal AN0 (RA0)
    ADCON1 = 0x80;        // Justification à droite, VREF = VDD
    ADCON0bits.ADON = 1;  // Activer l'ADC
}

// Lecture de l'ADC
unsigned int read_adc(void) {
    ADCON0bits.GO = 1;       // Démarrer la conversion
    while (ADCON0bits.GO);  // Attendre la fin de la conversion
    return ((ADRESH << 8) + ADRESL);  // Retourner la valeur 10 bits
}

// Fonction pour ajuster le duty cycle du PWM
void set_pwm_duty_cycle(float duty_cycle_ratio) {
    // (PWMxDCH:PWMxDCL<7:6>)=DutyCycleRatio×4×(PR2+1)
    unsigned int pwm_value = (unsigned int)(duty_cycle_ratio * 4.0 * (PR2 + 1));  // Equation 17-3 page 179

    PWM4DCH = (pwm_value >> 2) & 0xFF;  // Stockage des 8 bits de poids fort
    PWM4DCL = (pwm_value << 6) & 0xC0;  // Stockage des 2 bits de poids faible
}

// Met à jour le duty cycle de la LED en fonction du potentiomètre
void update_pwm_from_adc(void) {
    unsigned int adc_value = read_adc();  // Lire l'ADC
    float duty_cycle = (float)adc_value / 1024;  // Convertir en rapport cyclique (0.0 - 1.0)
    set_pwm_duty_cycle(duty_cycle);
}

// Fonction de configuration initiale
void setup(void) {
    init_ports();
    init_adc();
    init_pwm4();
}

// Boucle principale
void main(void) {
    setup();

    while (1) {
        update_pwm_from_adc();  // Ajuster la luminosité de la LED en fonction du bouton
    }
}