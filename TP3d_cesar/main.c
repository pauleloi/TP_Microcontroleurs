#include "config_uart.h"
#include "configbits.h"
#include "lcd.h"
#include "spi.h"
#include <xc.h>

#define _XTAL_FREQ 8000000  // Fréquence du microcontrôleur

// **Définition du bouton**
#define BOUTON_TRIS TRISBbits.TRISB0  // Bouton sur RB0
#define BOUTON_PORT PORTBbits.RB0  // Lecture de l'état du bouton
#define BOUTON_INT_FLAG INTCONbits.INTF  // Flag d'interruption du bouton

// **Variables globales**
volatile int decalage = 0;  // Valeur du décalage
volatile int count = 0;  // Compteur Timer2
volatile int mode_chiffrement = 0;  // 0 = "dechiffrer", 1 = "chiffrer"

// **Initialisation des ports pour l'ADC (Potentiomètre sur RA0)**
void init_adc(void) {
    TRISA |= (1 << 0);  // RA0 en entrée
    ANSELA |= (1 << 0); // Activer mode analogique sur RA0

    ADCON0 = (0 << 2);  // Sélectionner le canal AN0 (RA0)
    ADCON1 = 0x80;  // Justification à droite, VREF = VDD
    ADCON0bits.ADON = 1;  // Activer l'ADC
}

// **Chiffrement César pour lettres et chiffres**
char cesar_encrypt(char letter, int shift) {
    if (letter >= 'A' && letter <= 'Z') {
        letter = letter + 32; // Convertir en minuscule
    }

    if (letter >= 'a' && letter <= 'z') {
        return ((letter - 'a' + shift) % 26) + 'a';  // Décalage sur 26 lettres
    }

    if (letter >= '0' && letter <= '9') {
        return ((letter - '0' + shift) % 10) + '0';  // Décalage sur 10 chiffres
    }

    return letter;  // Ne pas modifier les caractères non alphabétiques
}

// **Initialisation du bouton avec interruption**
void init_button_interrupt(void) {
    BOUTON_TRIS = 1;  // RB0 en entrée
    ANSELBbits.ANSB0 = 0;  // Désactiver l'entrée analogique sur RB0

    INTCONbits.INTE = 1;  // Activer l'interruption externe sur RB0
    OPTION_REGbits.INTEDG = 0;  // Détection sur front descendant (appui bouton)
    INTCONbits.GIE = 1;  // Activer les interruptions globales
}

// **Initialisation du Timer2 (1ms)**
void init_timer2_interrupt(void) {
    T2CONbits.T2CKPS = 0b10;  // Prescaler 16
    PR2 = 124;  // Valeur PR2 pour 1ms

    PIE1bits.TMR2IE = 1;  // Activer l'interruption Timer2
    INTCONbits.PEIE = 1;  // Activer interruptions périphériques
    INTCONbits.GIE = 1;   // Activer interruptions globales

    T2CONbits.TMR2ON = 1;  // Démarrer Timer2
}

// **Lecture de l'ADC**
unsigned int read_adc(void) {
    ADCON0bits.GO = 1;  // Démarrer la conversion
    while (ADCON0bits.GO);  // Attendre la fin de la conversion
    return ((ADRESH << 8) + ADRESL);  // Retourner la valeur 10 bits
}

// **Conversion d'un entier en chaîne de caractères**
void intToStr(int value, char *str) {
    str[0] = (value / 10) + '0';  // Chiffre des dizaines
    str[1] = (value % 10) + '0';  // Chiffre des unités
    str[2] = '\0';  // Fin de chaîne
}

// **Routine d'Interruption**
void __interrupt() isr(void) {
    // **Interruption du Timer2**
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;  // Reset flag d'interruption

        count++;
        if (count >= 100) {  // Mise à jour toutes les 100ms
            count = 0;

            // Lecture de l'ADC et mise à jour du décalage
            unsigned int adc_value = read_adc();
            decalage = (adc_value * 26) / 1024;  // Mise à l'échelle 0 - 25

            // Conversion en chaîne et affichage sur LCD
            char decalageStr[3];
            intToStr(decalage, decalageStr);

            LCD_GoTo(1, 11);
            LCD_WriteString(decalageStr);
        }
    }

    // **Interruption du bouton S1 (RB0)**
    if (BOUTON_INT_FLAG) {
        BOUTON_INT_FLAG = 0;  // Réinitialiser le flag d'interruption

        mode_chiffrement = !mode_chiffrement;  // Inversion du mode

        // Mise à jour du texte sur l'écran LCD
        LCD_GoTo(0, 0);  // Ligne 1, colonne 0
        LCD_WriteString(mode_chiffrement ? "chiffrer    " : "dechiffrer  ");  // Effacement propre avec espaces
    }
}

// **Programme principal**
void main(void) {
    // Initialisation des modules
    SPI_Initialize();
    SPI_InitializePins();
    LCD_InitializePins();
    LCD_Initialize();
    init_adc();
    init_button_interrupt();
    init_timer2_interrupt();

    // Initialisation de l'UART
    init_uart();

    // Affichage initial sur le LCD
    LCD_Clear();
    LCD_GoTo(0, 0);
    LCD_WriteString("dechiffrer");

    LCD_GoTo(1, 0);
    LCD_WriteString("decalage : 0");

    // Message d'accueil dans le terminal série
    WriteTable("\n\rMode Echo : Tapez un caractere\n\r");
    Write('>');

    mode_chiffrement = 0;

    while (1) {
        char receivedChar = Read();  // Lire le caractère envoyé depuis le PC

        if (receivedChar >= 'A' && receivedChar <= 'Z') {
            receivedChar = receivedChar + 32;  // Conversion en minuscule
        }

        if (mode_chiffrement) {
            char encryptedChar = cesar_encrypt(receivedChar, decalage);
            Write(encryptedChar);  // Écriture du caractère chiffré
        } else {
            Write(receivedChar);  // Mode normal, on renvoie le caractère en minuscule
        }
    }
}
