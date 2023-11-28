//// POTENTIAL REWRITE/RESTRUCTURE

// IMPORT REQUIRED LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
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


// READ BYTES AT OFFSET
void readBytes(char *filename, off_t offset, char *buffer, ssize_t bytesToRead) {
    int fd = open(filename, O_RDONLY);        // Open file
    lseek(fd, offset, SEEK_SET);          // Seek to the byte position for the bootsector
    read(fd, buffer, bytesToRead);        // Read the file from start the the end of the boot sector
    close(fd);                            // Close file
}

// RETURN CLUSTER AT OFFSET
u_int16_t getCluster(int fd, char *filename, off_t offset) {
    u_int16_t cluster;
    readBytes(fd, filename, offset, &cluster, sizeof(u_int16_t));
    return cluster;
}

// PRINT BOOT SECTOR FIELDS
void printBootSector(BootSector *bootSector) {
    // Print bootsector fields (debug)

    // printf("BS_jmpBoot: ");
    // for (size_t i = 0; i < sizeof(bootSector->BS_jmpBoot); i++)
    // {
    //     printf("%d ", bootSector->BS_jmpBoot[i]);
    // }
    // printf("\n");
    // printf("BS_OEMName: %s\n", bootSector->BS_OEMName);
    printf("BPB_BytsPerSec: %d\n", bootSector->BPB_BytsPerSec);
    printf("BPB_SecPerClus: %d\n", bootSector->BPB_SecPerClus);
    printf("BPB_RsvdSecCnt: %d\n", bootSector->BPB_RsvdSecCnt);
    printf("BPB_NumFATs: %d\n", bootSector->BPB_NumFATs);
    printf("BPB_RootEntCnt: %d\n", bootSector->BPB_RootEntCnt);
    printf("BPB_TotSec16: %d\n", bootSector->BPB_TotSec16);
    // printf("BPB_Media: %d\n", bootSector->BPB_Media);
    printf("BPB_FATSz16: %d\n", bootSector->BPB_FATSz16);
    // printf("BPB_SecPerTrk: %d\n", bootSector->BPB_SecPerTrk);
    // printf("BPB_NumHeads: %d\n", bootSector->BPB_NumHeads);
    // printf("BPB_HiddSec: %d\n", bootSector->BPB_HiddSec);
    printf("BPB_TotSec32: %d\n", bootSector->BPB_TotSec32);
    // printf("BS_DrvNum: %d\n", bootSector->BS_DrvNum);
    // printf("BS_Reserved1: %d\n", bootSector->BS_Reserved1);
    // printf("BS_BootSig: %d\n", bootSector->BS_BootSig);
    // printf("BS_VolID: %d\n", bootSector->BS_VolID);
    printf("BS_VolLab: %s\n", bootSector->BS_VolLab);
    // printf("BS_FilSysType: %s\n", bootSector->BS_FilSysType);

    printf("\n");
}





// MAIN FUNCTION
int main() {
    char filename[] = "fat16.img";
    BootSector bootSector;
    
    // TASK 2
    readBytes(filename, 0, &bootSector, sizeof(BootSector)); // Runs readBytes function to open the file, seek to the bytes, and read them to the bootSector struct directly
    printBootSector(&bootSector);

    // TASK 3



}