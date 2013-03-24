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
#include "fatfs.h"

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

int main(void){
    uart_init(57600);
    spi_init(2);
    SDcard_init();
    spi_init(0);

    mount_disk();
    file_create("ZETALOGS.TXT");
    file_open();
    file_append("Hello World this is a test logfile!\n");
    file_append("If you can read this file, everything went fine!\n");
    file_append("And because I am very curious, I'm adding a third line!\n");
    file_append("This can be continued until the file has reached it's max length of 512 bytes..\n I am working on it!");
    file_close();
    /*
    uart_puts("Printing SDRdata\n");
    for(unsigned int b = 0; b < 512; b++){
            uart_putc(SDRdata[b]);
    }
    */

    TRISCbits.TRISC0 = 0;
    LATCbits.LATC0 = 1;
    while(1){
    }

    return 0;
}