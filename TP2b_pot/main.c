#include "configbits.h"
#include <xc.h>

#define _XTAL_FREQ 8000000

// Initialisation des ports
void init_ports(void) {
    // Configuration des LEDs en sortie
    TRISD = 0x00;  // PORTD en sortie (D1-D4)
    TRISB = 0x00;  // PORTB en sortie (D5-D8)
    LATD = 0x00;   // Éteindre les LEDs D1-D4
    LATB = 0x00;   // Éteindre les LEDs D5-D8

    // Configuration du potentiomètre en entrée analogique
    TRISA |= 0x01;  // Active uniquement le bit 0 de TRISA (RA0 en entrée)
    ANSELA |= 0x01; // Active uniquement le bit 0 de ANSELA (RA0 en mode analogique)
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

// Convertir la valeur de l'ADC en affichage LED
void update_leds(void) {
    // Calcul LED à allumer selon potentiomètre
    unsigned int adc_value = read_adc();
    unsigned char led_position = (adc_value * 8) / 1024;

    // Réinitialiser les LEDs
    LATD = 0x00;
    LATB = 0x00;

    // Séparer l'affichage entre PORTD et PORTB
    if (led_position < 4) {
        LATD = (1 << led_position);  // D1-D4
    } else {
        LATB = (1 << (led_position - 4));  // D5-D8
    }
}

// Configuration initiale
void setup(void) {
    init_ports();
    init_adc();
}

// Boucle principale
void main(void) {
    setup();

    while (1) {
        update_leds();
    }
}