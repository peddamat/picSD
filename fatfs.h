/* 
 * File:   fatfs.h
 * Author: elia
 *
 * Created on March 23, 2013, 5:03 PM
 */

#ifndef FATFS_H
#define	FATFS_H

void mount_disk(void);
void file_create(const unsigned char* filename);
void file_open(void);
void file_close(void);
void file_append(const unsigned char* string);
void file_update_size(void);

#endif	/* FATFS_H */

