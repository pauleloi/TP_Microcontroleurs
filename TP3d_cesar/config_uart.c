#include "configbits.h"
#include <xc.h>

#define _XTAL_FREQ 8000000  // Fréquence du microcontrôleur

// **Initialisation de la liaison UART**
void init_uart(void) {
    // **1. Configuration des broches UART**
    RC6PPS = 0x14;  // RC6 = TX1 (Transmetteur)
    RXPPS = 0x17;   // RC7 = RX1 (Récepteur)

    // **2. Configuration des ports RC6 et RC7**
    TRISCbits.TRISC6 = 0;  // RC6 en sortie (TX)
    TRISCbits.TRISC7 = 1;  // RC7 en entrée (RX)
    ANSELCbits.ANSC6 = 0;  // Désactiver l'entrée analogique sur RC6
    ANSELCbits.ANSC7 = 0;  // Désactiver l'entrée analogique sur RC7

    // **3. Configuration du baudrate à 9600 bauds**
    TX1STAbits.SYNC = 0;   // Mode asynchrone
    TX1STAbits.BRGH = 1;   // High Baud Rate Select
    BAUD1CONbits.BRG16 = 0; // Utilisation de SP1BRGL uniquement

    // Calcul de SP1BRG pour 9600 bauds avec FOSC = 8 MHz
    SP1BRGL = 51;  // Partie basse
    SP1BRGH = 0;   // Partie haute

    // **4. Activation du module EUSART**
    RC1STAbits.SPEN = 1;  // Activation du module UART
    TX1STAbits.TXEN = 1;  // Activation du transmetteur
    RC1STAbits.CREN = 1;  // Activation du récepteur
}

//Wait ~ x millisecond
void delai_approx(int milSec) {
    for(int i=0; i<milSec; i++)
        __delay_ms(1);
}

// Envoie 'length' caractères depuis le tableau
void Write(char table) {
    TX1REG = table;
}

// Envoie 'length' caractères depuis le tableau
void WriteTable(const char *table) {
    unsigned int i = 0;
    while(table[i] != '\0') {
        TX1REG = table[i];
        delai_approx(3); // Wait 3 ms
        i++;
    }
}

// **Lire un caractère depuis l'UART**
char Read(void) {
    while (!PIR1bits.RCIF);  // Attente qu'un caractère soit reçu
    return RC1REG;  // Retourner le caractère reçu
}