#include <xc.h>
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
    FAT_address[1] = FAT_address[0] + (sectors_FAT * sector_size);

    // calculate root directory table start address
    rootdir_start = FAT_address[1] + (sectors_FAT * sector_size);

    // calculate start address of data region
    datareg_start = rootdir_start + (root_entries * 32);
}

void file_create(const unsigned char* filename){
    // read first sector of FAT1
    SDcard_read_block(FAT_address[0]);

    // find first unallocated cluster
    uart_puts("Searching first unallocated cluster...");
    unsigned int cluster;
    for(cluster = 0; cluster < 0xFFFF; cluster++){
        if( (SDRdata[cluster*2] == 0) && (SDRdata[(cluster*2)+1] == 0) ) break;
    }
    uart_puts("done!\n");

    // read first sector of root directory table
    SDcard_read_block(rootdir_start);

    // search first unallocated entry
    uart_puts("Searching first unallocated root table entry...");
    unsigned int entry_addr;
    for(entry_addr = 0; entry_addr < 0xFFFF; entry_addr += 33){
        if(SDRdata[entry_addr] == 0xE5) break;
    }
    uart_puts("done!\n");

    uart_puts("Copying input buffer...");
    // copy read table block to SDWdata
    for(unsigned int i = 0; i < 512; i++){
        SDWdata[i] = SDRdata[i];
    }
    uart_puts("done!\n");

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
    //SDWdata[entry_addr+0x1A] = cluster & 0xFF;
    //SDWdata[entry_addr+0x1B] = (cluster>>8) & 0xFF;
    // no cluster allocated yet
    SDWdata[entry_addr+0x1A] = 0x00;
    SDWdata[entry_addr+0x1B] = 0x00;

    // set initial file size to zero
    SDWdata[entry_addr+0x1C] = 0x00;
    SDWdata[entry_addr+0x1D] = 0x00;
    SDWdata[entry_addr+0x1E] = 0x00;
    SDWdata[entry_addr+0x1F] = 0x00;

    // write data block
    SDcard_write_block(rootdir_start);
}
