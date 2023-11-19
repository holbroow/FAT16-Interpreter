#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

typedef struct __attribute__((__packed__)) {
    u_int8_t BS_jmpBoot[ 3 ];       // x86 jump instr. to boot code
    u_int8_t BS_OEMName[ 8 ];       // What created the filesystem
    u_int16_t BPB_BytsPerSec;       // Bytes per Sector
    u_int8_t BPB_SecPerClus;        // Sectors per Cluster
    u_int16_t BPB_RsvdSecCnt;       // Reserved Sector Count
    u_int8_t BPB_NumFATs;           // Number of copies of FAT
    u_int16_t BPB_RootEntCnt;       // FAT12/FAT16: size of root DIR
    u_int16_t BPB_TotSec16;         // Sectors, may be 0, see below
    u_int8_t BPB_Media;             // Media type, e.g. fixed
    u_int16_t BPB_FATSz16;          // Sectors in FAT (FAT12 or FAT16)
    u_int16_t BPB_SecPerTrk;        // Sectors per Track
    u_int16_t BPB_NumHeads;         // Number of heads in disk
    u_int32_t BPB_HiddSec;          // Hidden Sector count
    u_int32_t BPB_TotSec32;         // Sectors if BPB_TotSec16 == 0
    u_int8_t BS_DrvNum;             // 0 = floppy, 0x80 = hard disk
    u_int8_t BS_Reserved1;          //
    u_int8_t BS_BootSig;            // Should = 0x29
    u_int32_t BS_VolID;             // 'Unique' ID for volume
    u_int8_t BS_VolLab[ 11 ];       // Non zero terminated string
    u_int8_t BS_FilSysType[ 8 ];    // e.g. 'FAT16 ' (Not 0 term.)
} BootSector;

ssize_t readBytes(int fd, off_t offset, char *buffer, ssize_t bytesToRead) {
    fd = open("fat16.img", O_RDONLY);               // Open file
    lseek(fd, offset, SEEK_SET);                    // Seek to the byte position for the bootsector
    return read(fd, buffer, bytesToRead);           // Read the file from start the the end of the boot sector
    close(fd);                                      // Close file
}

int main() {
    int fd;                                     // The fat16 image
    off_t offset = 0;                           // How many bytes from the start of the file to begin
    size_t bytesToRead = sizeof(BootSector);    // How many bytes to be read from the file
    ssize_t bytesRead;                          // How many bytes were actually read from the file
    char buffer[bytesToRead];                   // Buffer where the file bytes are temporarily read to
    BootSector bootSector;                      // Struct of BootSector fields

    size_t bootSectorByteSize = sizeof(BootSector);


    bytesRead = readBytes(fd, offset, buffer, bytesToRead);     // Runs readBytes functions to open the file, seek to the bytes, and read them
    memcpy(&bootSector, buffer, bytesToRead);                   // Copy its contents to the correct struct fields


    // Print bootsector fields (debug)
    printf("BS_jmpBoot: ");
    for (size_t i = 0; i < sizeof(bootSector.BS_jmpBoot); i++) {
        printf("%02X ", (unsigned char)bootSector.BS_jmpBoot[i]);
    }
    printf("\n");
    printf("BS_OEMName: ");
    for (size_t i = 0; i < sizeof(bootSector.BS_OEMName); i++) {
        printf("%02X ", (unsigned char)bootSector.BS_OEMName[i]);
    }
    printf("\n");
    printf("BPB_BytsPerSec: %02X\n", (unsigned char)bootSector.BPB_BytsPerSec);
    printf("BPB_SecPerClus: %02X\n", (unsigned char)bootSector.BPB_SecPerClus);
    printf("BPB_RsvdSecCnt: %02X\n", (unsigned char)bootSector.BPB_RsvdSecCnt);
    printf("BPB_NumFATs: %02X\n", (unsigned char)bootSector.BPB_NumFATs);
    printf("BPB_RootEntCnt: %02X\n", (unsigned char)bootSector.BPB_RootEntCnt);
    printf("BPB_TotSec16: %02X\n", (unsigned char)bootSector.BPB_TotSec16);
    printf("BPB_Media: %02X\n", (unsigned char)bootSector.BPB_Media);
    printf("BPB_FATSz16: %02X\n", (unsigned char)bootSector.BPB_FATSz16);
    printf("BPB_SecPerTrk: %02X\n", (unsigned char)bootSector.BPB_SecPerTrk);
    printf("BPB_NumHeads: %02X\n", (unsigned char)bootSector.BPB_NumHeads);
    printf("BPB_HiddSec: %02X\n", (unsigned char)bootSector.BPB_HiddSec);
    printf("BPB_TotSec32: %02X\n", (unsigned char)bootSector.BPB_TotSec32);
    printf("BS_DrvNum: %02X\n", (unsigned char)bootSector.BS_DrvNum);
    printf("BS_Reserved1: %02X\n", (unsigned char)bootSector.BS_Reserved1);
    printf("BS_BootSig: %02X\n", (unsigned char)bootSector.BS_BootSig);
    printf("BS_VolID: %02X\n", (unsigned char)bootSector.BS_VolID);
    printf("BS_VolLab: ");
    for (size_t i = 0; i < sizeof(bootSector.BS_VolLab); i++) {
        printf("%02X ", (unsigned char)bootSector.BS_VolLab[i]);
    }
    printf("\n");
    printf("BS_FilSysType: ");
    for (size_t i = 0; i < sizeof(bootSector.BS_FilSysType); i++) {
        printf("%02X ", (unsigned char)bootSector.BS_FilSysType[i]);
    }
    printf("\n\n");


    // Task 3 - Load a copy of the first FAT into memory and produce an ordered
    // list of all clusters that make up a fale given the starting cluster number etc.
    
    





}
