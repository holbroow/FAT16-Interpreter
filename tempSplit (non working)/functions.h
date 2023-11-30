#ifndef FUNCTIONS
#define FUNCTIONS

// IMPORT REQUIRED LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// DATA STRUCTURES
typedef struct __attribute__((__packed__)) {
    u_int8_t BS_jmpBoot[3];    // x86 jump instr. to boot code
    u_int8_t BS_OEMName[8];    // What created the filesystem
    u_int16_t BPB_BytsPerSec;  // Bytes per Sector
    u_int8_t BPB_SecPerClus;   // Sectors per Cluster
    u_int16_t BPB_RsvdSecCnt;  // Reserved Sector Count
    u_int8_t BPB_NumFATs;      // Number of copies of FAT
    u_int16_t BPB_RootEntCnt;  // FAT12/FAT16: size of root DIR
    u_int16_t BPB_TotSec16;    // Sectors, may be 0, see below
    u_int8_t BPB_Media;        // Media type, e.g. fixed
    u_int16_t BPB_FATSz16;     // Sectors in FAT (FAT12 or FAT16)
    u_int16_t BPB_SecPerTrk;   // Sectors per Track
    u_int16_t BPB_NumHeads;    // Number of heads in disk
    u_int32_t BPB_HiddSec;     // Hidden Sector count
    u_int32_t BPB_TotSec32;    // Sectors if BPB_TotSec16 == 0
    u_int8_t BS_DrvNum;        // 0 = floppy, 0x80 = hard disk
    u_int8_t BS_Reserved1;     //
    u_int8_t BS_BootSig;       // Should = 0x29
    u_int32_t BS_VolID;        // 'Unique' ID for volume
    u_int8_t BS_VolLab[11];    // Non zero terminated string
    u_int8_t BS_FilSysType[8]; // e.g. 'FAT16 ' (Not 0 term.)
} BootSector;

typedef struct __attribute__((__packed__)) {
    u_int8_t DIR_Name[11];     // Non zero terminated string
    u_int8_t DIR_Attr;         // File attributes
    u_int8_t DIR_NTRes;        // Used by Windows NT, ignore
    u_int8_t DIR_CrtTimeTenth; // Tenths of sec. 0...199
    u_int16_t DIR_CrtTime;     // Creation Time in 2s intervals
    u_int16_t DIR_CrtDate;     // Date file created
    u_int16_t DIR_LstAccDate;  // Date of last read or write
    u_int16_t DIR_FstClusHI;   // Top 16 bits file's 1st cluster
    u_int16_t DIR_WrtTime;     // Time of last write
    u_int16_t DIR_WrtDate;     // Date of last write
    u_int16_t DIR_FstClusLO;   // Lower 16 bits file's 1st cluster
    u_int32_t DIR_FileSize;    // File size in bytes
} DirectoryEntry;

// FUNCTION DECLARATIONS
void readBytes(char *filename, off_t offset, void *buffer, ssize_t bytesToRead);
u_int16_t getCluster(char *filename, off_t offset);
void printFields(char *filename, BootSector *bootSector, size_t bootSectorSize);
void produceClusters(char *filename, BootSector *bootSector, size_t bootSectorSize);
void listDir(char *filename, BootSector *bootSector, size_t bootSectorSize, off_t dirOffset);
void openEntry(char *filename, BootSector *bootSector, size_t bootSectorSize, DirectoryEntry chosenEntry);

#endif
