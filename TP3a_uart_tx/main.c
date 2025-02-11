#include "configbits.h"
#include <xc.h>

#define _XTAL_FREQ 8000000  // Fréquence du microcontrôleur

// **Initialisation de la liaison UART**
void init_uart(void) {
    // **1. Configuration des broches pour la communication UART**
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

// Envoye un caractère sur l'UART
void UART_SendChar(char c) {
    while(!TX1STAbits.TRMT); // Attendre que l'UART soit prêt à envoyer
    TX1REG = c;              // Envoyer le caractère
}

// Envoie 'length' caractères depuis le tableau
void UART_SendTable(const char *table, unsigned int length) {
    unsigned int i;
    for(i = 0; i < length; i++){
        UART_SendChar(table[i]);
    }
}

void main(void) {
    // Configuration UART
    init_uart();

    // Définition du tableau de caractères à envoyer
    const char message[] = "Hello, World!\n\r";
    // Calcul de la longueur du message (sans compter le caractère nul)
    unsigned int message_length = sizeof(message) - 1;

    while(1) {
        // Envoyer le contenu du tableau et s'arrêter après le dernier caractère
        UART_SendTable(message, message_length);
        __delay_ms(1000);
    }
}