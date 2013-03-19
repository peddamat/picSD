#ifndef _XTAL_FREQ
#define _XTAL_FREQ 16000000UL
#endif

#include <xc.h>
#include "uart.h"

void uart_init(unsigned int baudrate){

    unsigned int BRG_val = ((_XTAL_FREQ/16)/(baudrate-1));
    SPBRGH1 = BRG_val>>8;
    SPBRG1 = BRG_val;

    TXSTAbits.BRGH = 1; // high speed mode
    TRISCbits.RC6 = 1;
    TRISCbits.RC7 = 1;

    TXSTA1bits.TXEN = 1;
    RCSTA1bits.SPEN = 1;
}

void uart_putc(unsigned char c){
    while(!TXSTAbits.TRMT);
    TXREG1 = c;
}

void uart_puts(const unsigned char *s){
    while(*s){
        uart_putc(*s);
        s++;
    }
}
