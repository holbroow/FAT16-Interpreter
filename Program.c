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

// Task 1
ssize_t readBytes(int fd, char *filename, off_t offset, char *buffer, ssize_t bytesToRead) {
    fd = open(filename, O_RDONLY);                  // Open file
    lseek(fd, offset, SEEK_SET);                    // Seek to the byte position for the bootsector
    return read(fd, buffer, bytesToRead);           // Read the file from start the the end of the boot sector
    close(fd);                                      // Close file
}

// Task 2
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

// Task 3
void produceClusters(int fd, char *filename, BootSector *bootSector, size_t bootSectorSize) {
    ssize_t bytesRead;                                                                                              // How many bytes were actually read from the file
    u_int16_t retrievedCluster;                                                                                     // The first cluster of the FAT
    u_int16_t previousCluster;                                                                                      // The most recent cluster retrieved from the file
    off_t initialFATOffset = bootSector->BPB_RsvdSecCnt * bootSector->BPB_BytsPerSec;                               // The offset at which we will start reading (the position of the first fat cluster)
    size_t clusterCount = 0;                                                                                        // Counter for the amount of clusters read from the file
    u_int16_t FATArray[bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec/2];                                     // Array for the first FAT


    printf("Offset for first FAT byte: %d\n", initialFATOffset); // debug - Print the FAT offset to monitor the functions read start point
    bytesRead = readBytes(fd, filename, initialFATOffset, FATArray, bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec);

    printf("First cluster list: ");
    for (size_t i = 6; i < bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec/2; i++)
    {
        if (FATArray[i] >= 0xFFF8) {
            printf("%04X\n", FATArray[i]);
            break;
        }
        printf("%04X - ", FATArray[i]);
    }
    
    printf("\n");
}

// Task 4
void listFiles(int fd, char *filename, BootSector *bootSector, size_t bootSectorSize) {
    typedef struct __attribute__ ((__packed__)) {
        u_int8_t DIR_Name[ 11 ];             // Non zero terminated string
        u_int8_t DIR_Attr;                   // File attributes
        u_int8_t DIR_NTRes;                  // Used by Windows NT, ignore
        u_int8_t DIR_CrtTimeTenth;           // Tenths of sec. 0...199
        u_int16_t DIR_CrtTime;               // Creation Time in 2s intervals
        u_int16_t DIR_CrtDate;               // Date file created
        u_int16_t DIR_LstAccDate;            // Date of last read or write
        u_int16_t DIR_FstClusHI;             // Top 16 bits file's 1st cluster
        u_int16_t DIR_WrtTime;               // Time of last write
        u_int16_t DIR_WrtDate;               // Date of last write
        u_int16_t DIR_FstClusLO;             // Lower 16 bits file's 1st cluster
        u_int32_t DIR_FileSize;              // File size in bytes
    } DirectoryEntry;

    ssize_t bytesRead;
    off_t rootDIROffset = (bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16)) * bootSector->BPB_BytsPerSec;
    char directoryBuffer[32];
    //DirectoryEntry *directories[bootSector->BPB_RootEntCnt];
    DirectoryEntry **directories = malloc(bootSector->BPB_RootEntCnt * sizeof(DirectoryEntry *));

 
    for (size_t i = 0; i < bootSector->BPB_RootEntCnt; i++)
    {
        //DirectoryEntry entry1;
        DirectoryEntry *entry = malloc(sizeof(DirectoryEntry));
        bytesRead = readBytes(fd, filename, rootDIROffset + (i*32), entry, sizeof(DirectoryEntry));
        directories[i] = entry;
    }

    for (size_t i = 0; i < sizeof(directories); i++)
    {
        for (size_t j = 0; j < sizeof(directories[i]->DIR_Name); j++) {
            printf("%c", directories[i]->DIR_Name[j]);
        }
        printf("\n");
        printf("DIR_Attr: %08X\n", directories[i]->DIR_Attr);
        printf("DIR_NTRes: %08X\n", directories[i]->DIR_NTRes);
        printf("DIR_CrtTimeTenth: %08X\n", directories[i]->DIR_CrtTimeTenth);
        printf("DIR_CrtTime: %08X\n", directories[i]->DIR_CrtTime);
        printf("DIR_CrtDate: %08X\n", directories[i]->DIR_CrtDate);
        printf("DIR_LstAccDate: %08X\n", directories[i]->DIR_LstAccDate);
        printf("DIR_FstClusHI: %08X\n", directories[i]->DIR_FstClusHI);
        printf("DIR_WrtTime: %08X\n", directories[i]->DIR_WrtTime);
        printf("DIR_WrtDate: %08X\n", directories[i]->DIR_WrtDate);
        printf("DIR_FstClusLO: %08X\n", directories[i]->DIR_FstClusLO);
        printf("DIR_FileSize: %08X\n", directories[i]->DIR_FileSize);
    }
}



int main() {
    int fd;                                         // The file descriptor
    char filename[] = "fat16.img";                  // The fat16 image filename
    BootSector bootSector;                          // Struct of BootSector field
    size_t bootSectorSize = sizeof(bootSector);     // Size of BootSector in bytes


    // Task 2
    printf("Task 2\n");
    printFields(fd, filename, &bootSector, bootSectorSize);         // Print all fields of the boot sector


    // Task 3
    printf("Task 3\n");
    produceClusters(fd, filename, &bootSector, bootSectorSize);     // Load a copy of first FAT into memory, and print an ordered list of clusters


    // Task 4
    /*
        For each file, output:
        - First/Starting Cluster
        - Last modified (Time and Date)
        - File attributes (single letter for each, hyphen for unset flag)
        - File length
        - Filename

        !! (Formatted neatly in columns) !!
    */
    printf("Task 4\n");
    listFiles(fd, filename, &bootSector, bootSectorSize);           // Output a list of files in the root directory


}
