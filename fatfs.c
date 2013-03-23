#include <xc.h>
#include "fatfs.h"
#include "sdcard.h"

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
