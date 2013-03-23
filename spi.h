/* 
 * File:   spi.h
 * Author: Torrentula
 *
 * Created on 11. März 2013, 16:30
 */

#ifndef SPI_H
#define	SPI_H

void spi_init(unsigned char speed_mode);
void spi_send(unsigned char* data, unsigned int length);
void spi_receive(unsigned char* data, unsigned int length);

#endif	/* SPI_H */

