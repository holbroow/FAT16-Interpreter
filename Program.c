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

ssize_t readBytes(int fd, char *filename, off_t offset, char *buffer, ssize_t bytesToRead) {
    fd = open(filename, O_RDONLY);               // Open file
    lseek(fd, offset, SEEK_SET);                    // Seek to the byte position for the bootsector
    return read(fd, buffer, bytesToRead);           // Read the file from start the the end of the boot sector
    close(fd);                                      // Close file
}

void printFields(int fd, char *filename, BootSector *bootSector, size_t bootSectorSize) {
    off_t offset = 0;                               // How many bytes from the start of the file to begin
    size_t bytesToRead = sizeof(BootSector);        // How many bytes to be read from the file
    ssize_t bytesRead;                              // How many bytes were actually read from the file

    bytesRead = readBytes(fd, filename, offset, bootSector, bytesToRead);       // Runs readBytes functions to open the file, seek to the bytes, and read them to the bootSector struct directly


    // Print bootsector fields (debug)
    printf("BS_jmpBoot: ");
    for (size_t i = 0; i < sizeof(bootSector->BS_jmpBoot); i++) {
        printf("%d ", bootSector->BS_jmpBoot[i]);
    }
    printf("\n");
    printf("BS_OEMName: ");
    for (size_t i = 0; i < sizeof(bootSector->BS_OEMName); i++) {
        printf("%d ", bootSector->BS_OEMName[i]);
    }
    printf("\n");
    printf("BPB_BytsPerSec: %d\n", bootSector->BPB_BytsPerSec); // maybe an issue here
    printf("BPB_SecPerClus: %d\n", bootSector->BPB_SecPerClus);
    printf("BPB_RsvdSecCnt: %d\n", bootSector->BPB_RsvdSecCnt);
    printf("BPB_NumFATs: %d\n", bootSector->BPB_NumFATs);
    printf("BPB_RootEntCnt: %d\n", bootSector->BPB_RootEntCnt);
    printf("BPB_TotSec16: %d\n", bootSector->BPB_TotSec16);
    printf("BPB_Media: %d\n", bootSector->BPB_Media);
    printf("BPB_FATSz16: %d\n", bootSector->BPB_FATSz16);
    printf("BPB_SecPerTrk: %d\n", bootSector->BPB_SecPerTrk);
    printf("BPB_NumHeads: %d\n", bootSector->BPB_NumHeads);
    printf("BPB_HiddSec: %d\n", bootSector->BPB_HiddSec);
    printf("BPB_TotSec32: %d\n", bootSector->BPB_TotSec32);
    printf("BS_DrvNum: %d\n", bootSector->BS_DrvNum);
    printf("BS_Reserved1: %d\n", bootSector->BS_Reserved1);
    printf("BS_BootSig: %d\n", bootSector->BS_BootSig);
    printf("BS_VolID: %d\n", bootSector->BS_VolID);
    printf("BS_VolLab: ");
    for (size_t i = 0; i < sizeof(bootSector->BS_VolLab); i++) {
        printf("%d ", bootSector->BS_VolLab[i]);
    }
    printf("\n");
    printf("BS_FilSysType: ");
    for (size_t i = 0; i < sizeof(bootSector->BS_FilSysType); i++) {
        printf("%d ", bootSector->BS_FilSysType[i]);
    }
    printf("\n\n");
}

void produceClusters(int fd, char *filename, BootSector *bootSector, size_t bootSectorSize) {
    size_t bytesToRead = 8;        // How many bytes to be read from the file
    ssize_t bytesRead;                              // How many bytes were actually read from the file
    char buffer[bytesToRead];                       // Buffer where the file bytes are temporarily read to

    off_t initialFATOffset = bootSectorSize + (bootSector->BPB_RsvdSecCnt * bootSector->BPB_BytsPerSec);

    printf("Offset for first FAT byte: %d\n", initialFATOffset); // debug
    printf("BootSector size: %d\n", bootSectorSize); // debug

    bytesRead = readBytes(fd, filename, initialFATOffset, buffer, bytesToRead);

    for(size_t i = 0; i < sizeof(buffer); i++) {
        printf("Byte: %d\n", buffer[i]);
    }
}

int main() {
    int fd;                                         // The file descriptor
    char filename[] = "fat16.img";                  // The fat16 image filename
    BootSector bootSector;                          // Struct of BootSector field
    size_t bootSectorSize = sizeof(bootSector);     // Size of BootSector in bytes


    // Task 2

    printFields(fd, filename, &bootSector, bootSectorSize);       // Print all fields of the boot sector.


    // Task 3 - Load a copy of the first FAT into memory and produce an ordered
    // list of all clusters that make up a file given the starting cluster number etc.
    
    // A FAT  is simply an array of entries, one for each cluster, which states the next cluster in the
    // sequence that makes up a file. Thus, each file stored in the file system will have a chain of entries in
    // the FAT that identifies the clusters making up the file, and the order they appear in the sequence.

    produceClusters(fd, filename, &bootSector, bootSectorSize);     // Load a copy of first FAT into memory, and print an ordered list of clusters.



    


}
