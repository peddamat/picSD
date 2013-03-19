/* 
 * File:   uart.h
 * Author: Torrentula
 *
 * Created on 11. März 2013, 19:24
 */

#ifndef UART_H
#define	UART_H

void uart_init(unsigned int baud);
void uart_putc(unsigned char c);
void uart_puts(const unsigned char *s);

#endif	/* UART_H */

