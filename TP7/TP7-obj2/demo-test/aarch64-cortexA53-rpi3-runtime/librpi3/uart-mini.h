#ifndef UART_MINI_H
#define UART_MINI_H

#include <stdbool.h>
#include <stdint.h>

//==============================================================
// GPIO UART access on the RPi3
bool miniuart_init (uint32_t baudrate);
char miniuart_getc (void);
void miniuart_putc (const char c);
void miniuart_puts (const char *str);
void miniuart_puts_noend (const char *str);
uint32_t miniuart_read_uint32(void);

#endif
