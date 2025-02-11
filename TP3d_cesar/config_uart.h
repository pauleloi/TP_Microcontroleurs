#ifndef CONFIG_UART_H
#define CONFIG_UART_H
#include <xc.h>

#define _XTAL_FREQ 8000000

// **Déclaration des fonctions UART**
void init_uart(void);  // Initialisation de l'UART
void delai_approx(int milSec);  // Attente approximative en millisecondes
void Write(char table);  // Envoi d'un caractère via UART
void WriteTable(const char *table);  // Envoi d'une chaîne de caractères via UART
char Read(void);

#endif /* CONFIG_UART_H */