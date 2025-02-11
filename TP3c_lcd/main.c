#include "configbits.h"
#include <xc.h>
#include "lcd.h"
#include "spi.h"

#define _XTAL_FREQ 8000000  // Fréquence du microcontrôleur

void main(void) {
    // **1. Initialisation des modules**
    SPI_InitializePins();  // Initialisation des broches SPI
    LCD_InitializePins();  // Initialisation des broches LCD
    SPI_Initialize();      // Initialisation du SPI
    LCD_Initialize();      // Initialisation du LCD

    // **2. Affichage du message sur le LCD**
    LCD_Clear();     // Effacer l'écran
    LCD_GoTo(0, 0);  // Se positionner en ligne 1, colonne 0
    LCD_WriteString("Hello, World!");

    while (1) {
        // Boucle infinie
    }
}