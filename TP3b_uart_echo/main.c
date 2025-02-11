#include "configbits.h"
#include "config_uart.h"
#include <xc.h>


#define _XTAL_FREQ 8000000

// **Lire un caractère depuis l'UART**
char Read(void) {
    while (!PIR1bits.RCIF);  // Attente qu'un caractère soit reçu
    return RC1REG;  // Retourner le caractère reçu
}

void main(void) {
    // Initialisation de l'UART
    init_uart();

    // Message d'accueil dans le terminal série
    WriteTable("\n\rMode Echo : Tapez un caractere\n\r");
    Write('>');

    while (1) {
        char receivedChar = Read();  // Lire le caractère envoyé depuis le PC
        Write(receivedChar);         // Renvoyer le même caractère (écho)
    }
}