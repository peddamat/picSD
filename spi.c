#include <xc.h>
#include "spi.h"

void spi_init(void){
    // SDI1 set
    TRISCbits.TRISC4 = 1; // configure RC4 as input
    ANSELCbits.ANSC4 = 0; // enable digital input buffer on RC4
    // SS1 set
    TRISAbits.TRISA5 = 1;
    // SDO1 cleared
    // SCK1 cleared
    TRISCbits.TRISC5 = 0;
    TRISCbits.TRISC3 = 0;

    // SPI mode 0
    SSP1CON1bits.CKP = 0; // Idle state for clock is a low level
    SSP1STATbits.CKE = 1; // Transmit occurs on transition from active to Idle clock state
    SSP1STATbits.SMP = 1; // Input data sampled at end of data output time (took me 5 friggin' hours)
    SSP1CON1bits.SSPM = 0x02; // SPI Master mode, clock = FOSC/64
    SSP1CON1bits.SSPEN = 1; // enable MSSP1
}

void spi_send(unsigned char* data, unsigned int length){
    unsigned char tmp;
    while(length != 0){
	SSP1BUF = *data;
	while( !PIR1bits.SSP1IF ); // wait for buffer full
        PIR1bits.SSP1IF = 0; // clear SSP1IF
        tmp = SSP1BUF; // read out data
        length--;
	data++;
    }
}

void spi_receive(unsigned char* data, unsigned int length){
    while(length != 0){
	SSP1BUF = 0xFF;
	while( !PIR1bits.SSP1IF ); // wait for transmission complete
        while( !SSP1STATbits.BF ); // wait for buffer full
        PIR1bits.SSP1IF = 0; // clear SSP1IF
        *data = SSP1BUF;
	length--;
	data++;
    }
}