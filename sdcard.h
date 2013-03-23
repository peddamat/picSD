/* 
 * File:   sdcard.h
 * Author: elia
 *
 * Created on March 23, 2013, 4:39 PM
 */

#ifndef SDCARD_H
#define	SDCARD_H

#define SD_TIMEOUT 255

extern unsigned char SDRdata[512];
extern unsigned char SDWdata[512];

void SDcard_init(void);
void SDcard_read_block(unsigned long address);
unsigned char SDcard_get_response(unsigned char response);

#endif	/* SDCARD_H */