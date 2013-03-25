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
    
    sector_open();
    file_append("This line contains 32characters\n");
    file_append("This line also has 32characters\n");
    file_append("Well it is kinda sorta stupid but");
    file_append(" we have to write in 512 byte chu");
    file_append("nks. Convoluted and shiz but geez");
    file_append(" what ya gonna do?\n There ain't ");
    file_append("no other solution! Let's start th");
    file_append("e alphabet, shall we?          \n");
    file_append("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefg");
    file_append("hijklmnopqrstuvwxyz            \n");
    file_append("That worked a treat!           \n");
    file_append("                                 ");
    file_append("                                 ");
    file_append("                                 ");
    file_append("                                 ");
    file_append("                             \n\n");

    file_append("And now this is the second block ");
    file_append("of information starting in the ne");
    file_append("xt cluster. I can write out my bu");
    file_append("tt here because I will have to fi");
    file_append("ll 512 bytes to get this block wr");
    file_append("itten to the text file no other w");
    file_append("ay :( \n\n The standard log could");
    file_append(" look like this:             \n\n");
    file_append("                                 ");
    file_append("                                 ");
    file_append("                                 ");
    file_append("                                 ");
    file_append("                                 ");
    file_append("                                 ");
    file_append("                                 ");
    file_append("                                 ");
    file_update_size();
    
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