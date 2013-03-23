/* 
 * File:   main.c
 * Author: Torrentula
 *
 * Created on 10. März 2013, 21:51
 */
#define _XTAL_FREQ 16000000UL
#include <xc.h>
#include "uart.h"
#include "spi.h"
#include "sdcard.h"

#pragma config IESO=OFF         // Oscillator Switchover mode disabled
#pragma config FOSC=HSMP        // external medium power oscillator
#pragma config PRICLKEN=OFF     // Primary clock can be disabled by software
#pragma config FCMEN=ON         // Fail-Safe Clock Monitor enabled
#pragma config PLLCFG=OFF       // disable 4xPLL
#pragma config BOREN=SBORDIS    // disable software BOR
#pragma config BORV=190         // Brown-out voltage 1.9V
#pragma config PWRTEN=ON        // Power up timer enabled
#pragma config WDTEN=OFF        // WDT off
#pragma config MCLRE=EXTMCLR    // external MCLR pin enabled

// holds partition 1 PBR address
unsigned long part1_addr;

int main(void){
    uart_init(57600);
    spi_init(2);
    SDcard_init();

    spi_init(0);

    // read sector 0 from card
    SDcard_read_block(0x00000000);
    // calculate address of partition 1 PBR (first sector)
    // read parition 1 LBA from MBR
    // LBA 4 bytes at address 0x01be offset 0x08
    // Little Endian
    part1_addr = (unsigned long)SDRdata[0x01C6];
    part1_addr |= ((unsigned long)SDRdata[0x01C7]<<8);
    part1_addr |= ((unsigned long)SDRdata[0x01C8]<<16);
    part1_addr |= ((unsigned long)SDRdata[0x01C9]<<24);
    // multiply LBA by sector size to
    // get address of first sector of first partition
    part1_addr *= 0x00000200;
    
    // read first sector (PBR) of
    // partition 1
    SDcard_read_block(part1_addr);

    uart_puts("Printing SDRdata\n");
    for(unsigned int b = 0; b < 512; b++){
            uart_putc(SDRdata[b]);
    }

    TRISCbits.TRISC0 = 0;
    LATCbits.LATC0 = 1;
    while(1){
    }

    return 0;
}