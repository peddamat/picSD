#include <xc.h>
#include <stdlib.h>
#include "fatfs.h"
#include "sdcard.h"
#include "uart.h"

// holds partition 1 PBR address
unsigned long part1_addr;
// number of bytes per sector
unsigned int sector_size;
// number of sectors per cluster
unsigned char sectors_cluster;
// number of reserved sectors
unsigned int sectors_reserved;
// number of FAT copies
unsigned char FAT_copies;
// number of possible root entries
unsigned int root_entries;
// number of sectors per FAT
unsigned int sectors_FAT;

// FAT addresses
unsigned long FAT_address[2];
// root directory table start address
unsigned long rootdir_start;
// data region start address
unsigned long datareg_start;

// file specific variables
// starting cluster number of file
unsigned int cluster_number;
// starting sector of cluster address
unsigned long start_sector_address;
// sector address of current sector
unsigned long current_sector_address;
// current sector number
unsigned char current_sector = 0;
// entry address in root directory table
unsigned int entry_addr;
// number of byte in sector
unsigned int byte_number = 0;
// file length
unsigned long file_length = 0;

char address_buffer[10];

void mount_disk(void){
    // read sector 0 (MBR) from card
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
    
    // read out sector size in bytes
    sector_size = SDRdata[0x000B];
    sector_size |= SDRdata[0x000C]<<8;
    // read out sectors per cluster
    sectors_cluster = SDRdata[0x000D];
    // read out number of reserved sectors
    sectors_reserved = SDRdata[0x000E];
    sectors_reserved |= SDRdata[0x000F]<<8;
    // read out number of FATs
    FAT_copies = SDRdata[0x0010];
    // read out number of maximum root entries possible
    root_entries = SDRdata[0x0011];
    root_entries |= SDRdata[0x0012]<<8;
    // read out single FAT size in sectors
    sectors_FAT = SDRdata[0x0016];
    sectors_FAT |= SDRdata[0x0017]<<8;
    
    // calculate addresses of both FAT copies
    FAT_address[0] = part1_addr + (sectors_reserved * sector_size);
    FAT_address[1] = FAT_address[0] + ((unsigned long)sectors_FAT * (unsigned long)sector_size);

    // calculate root directory table start address
    rootdir_start = FAT_address[1] + ((unsigned long)sectors_FAT * (unsigned long)sector_size);

    // calculate start address of data region
    datareg_start = rootdir_start + ((unsigned long)root_entries * 32);
}

void file_create(const unsigned char* filename){
    // read first sector of FAT1
    SDcard_read_block(FAT_address[0]);

    // find first unallocated cluster
    uart_puts("Searching first unallocated cluster...");
    for(cluster_number = 0; cluster_number < 0xFFFF; cluster_number++){
        if( (SDRdata[cluster_number*2] == 0) && (SDRdata[(cluster_number*2)+1] == 0) ) break;
    }
    uart_puts("done!\n");

    // allocate cluster in FAT
    // copy read sector
    for(unsigned int i = 0; i < 512; i++){
        SDWdata[i] = SDRdata[i];
    }
    SDWdata[cluster_number*2] = 0xFF;
    SDWdata[(cluster_number*2)+1] = 0xFF;
    // write new FAT to both fat copies
    SDcard_write_block(FAT_address[0]);
    SDcard_write_block(FAT_address[1]);
    // calculate address of first sector in cluster
    start_sector_address = datareg_start + (((unsigned long)(cluster_number-2)) * (unsigned long)sectors_cluster * (unsigned long)sector_size);
    current_sector = 0;

    itoa(address_buffer, cluster_number, 10);
    uart_puts("Cluster number: ");
    uart_puts(address_buffer);
    uart_putc('\n');

    ltoa(address_buffer, start_sector_address, 16);
    uart_puts("Sector 0 address: ");
    uart_puts(address_buffer);
    uart_putc('\n');

    // read first sector of root directory table
    SDcard_read_block(rootdir_start);

    // search first unallocated entry
    uart_puts("Searching first unallocated root table entry...");
    for(entry_addr = 0; entry_addr < 0x0200; entry_addr += 32){
        if( (SDRdata[entry_addr] == 0xE5) || (SDRdata[entry_addr] == 0x00) ) break;
    }
    uart_puts("done!\n");

    ltoa(address_buffer, entry_addr, 16);
    uart_puts("Entry number: ");
    uart_puts(address_buffer);
    uart_putc('\n');

    // copy read table block to SDWdata
    for(unsigned int i = 0; i < 512; i++){
        SDWdata[i] = SDRdata[i];
    }

    // create file with filename
    unsigned char offset = 0;
    while(*filename){
        if(*filename != '.'){ // use '.' in filename as delimiter
            SDWdata[entry_addr+offset] = *filename;
            filename++;
            offset++;
        }
        else{
            filename++; // skip ''.' in filename
        }
    }

    // add attribute byte
    // archive flag set
    SDWdata[entry_addr+0x0B] = 0x20;

    // set reserved byte to 0
    SDWdata[entry_addr+0x0C] = 0x00;

    // set millisecond stamp
    SDWdata[entry_addr+0x0D] = 0x00;

    // set creation time
    // time 12:00:00
    SDWdata[entry_addr+0x0E] = 0x00;
    SDWdata[entry_addr+0x0F] = 0x60;

    // set creation data
    // date January 1, 2000
    SDWdata[entry_addr+0x10] = 0x21;
    SDWdata[entry_addr+0x11] = 0x24;

    // set last access date
    SDWdata[entry_addr+0x12] = 0x21;
    SDWdata[entry_addr+0x13] = 0x24;

    // reserved for FAT32
    SDWdata[entry_addr+0x14] = 0x00;
    SDWdata[entry_addr+0x15] = 0x00;

    // last write time
    SDWdata[entry_addr+0x16] = 0x00;
    SDWdata[entry_addr+0x17] = 0x60;

    // last write date
    SDWdata[entry_addr+0x18] = 0x21;
    SDWdata[entry_addr+0x19] = 0x24;

    // enter file starting cluster
    SDWdata[entry_addr+0x1A] = cluster_number & 0xFF;
    SDWdata[entry_addr+0x1B] = (cluster_number>>8) & 0xFF;

    // set initial file size to 0 bytes
    SDWdata[entry_addr+0x1C] = 0x00;
    SDWdata[entry_addr+0x1D] = 0x00;
    SDWdata[entry_addr+0x1E] = 0x00;
    SDWdata[entry_addr+0x1F] = 0x00;

    // write data block
    SDcard_write_block(rootdir_start);
}

void sector_open(void){
    // read sector of cluster
    current_sector_address = start_sector_address + ((unsigned long)current_sector * (unsigned long)sector_size);
    SDcard_read_block(current_sector_address);

    ltoa(address_buffer, current_sector_address, 16);
    uart_puts("Address of current sector: ");
    uart_puts(address_buffer);
    uart_putc('\n');

    // copy read sector to SDWdata
    for(unsigned int i = 0; i < 512; i++){
        SDWdata[i] = SDRdata[i];
    }
}

void sector_close(void){
    // write sector to card
    SDcard_write_block(current_sector_address);
}

void file_append(const unsigned char* string){
    // offset from beginning of write
    // (byte number)
    while(*string){
        if(byte_number < 512){
            SDWdata[byte_number] = *string;
            string++;
            byte_number++;
            // increase file length
            file_length++;
        }
        else{
            // save current sector
            sector_close();
            // reset byte counter
            byte_number = 0;
            // next sector is needed, load next sector
            current_sector++;
            sector_open();
        }
    }
}

void file_update_size(void){
    // read first sector of root directory table
    SDcard_read_block(rootdir_start);
    // copy read sector to SDWdata
    for(unsigned int i = 0; i < 512; i++){
        SDWdata[i] = SDRdata[i];
    }

    ltoa(address_buffer, file_length, 16);
    uart_puts("File size: ");
    uart_puts(address_buffer);
    uart_putc('\n');

    // set file size
    SDWdata[entry_addr+0x1C] = file_length & 0xFF;
    SDWdata[entry_addr+0x1D] = (file_length>>8) & 0xFF;
    SDWdata[entry_addr+0x1E] = (file_length>>16) & 0xFF;
    SDWdata[entry_addr+0x1F] = (file_length>>24) & 0xFF;
    // write first sector of root directory table
    SDcard_write_block(rootdir_start);
}
