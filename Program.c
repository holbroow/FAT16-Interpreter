#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

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

// Print u_int16_t values in binary (unused)
void printBinary16(int x) {
    for (int i = 0; i < 16; i++) {
        printf("%d", (x & 0x8000) >> 15);
        x <<= 1;
    }
}
// Print u_int8_t values in binary (unused)
void printBinary8(int x) {
    for (int i = 0; i < 8; i++) {
        printf("%d", (x & 0x8000) >> 15);
        x <<= 1;
    }
}


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
    printf("BS_OEMName: %s\n", bootSector->BS_OEMName);
    printf("BPB_BytsPerSec: %d\n", bootSector->BPB_BytsPerSec);
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
    printf("BS_VolLab: %s\n", bootSector->BS_VolLab);
    printf("BS_FilSysType: %s\n", bootSector->BS_FilSysType);
    
    printf("\n");
}

// Task 3
void produceClusters(int fd, char *filename, BootSector *bootSector, size_t bootSectorSize) {
    ssize_t bytesRead;                                                                                              // How many bytes were actually read from the file
    off_t initialFATOffset = bootSector->BPB_RsvdSecCnt * bootSector->BPB_BytsPerSec;                               // The offset at which we will start reading (the position of the first fat cluster)
    size_t clusterCount = 0;                                                                                        // Counter for the amount of clusters read from the file
    u_int16_t FATArray[bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec/2];                                     // Array for the first FAT
    bool firstFound = false;


    printf("Offset for first FAT byte: %d\n", initialFATOffset); // debug - Print the FAT offset to monitor the functions read start point
    bytesRead = readBytes(fd, filename, initialFATOffset, FATArray, bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec);

    printf("First cluster list: ");
    for (size_t i = 0; i < bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec/2; i++)
    {
        if (!firstFound && (FATArray[i] == 0000 || FATArray[i] >= 0xFFF8)) {
            //ignore value
        } else {
            if (FATArray[i] < 0xFFF8) {
                printf("%04X -", FATArray[i]);
                if(!firstFound) {
                    firstFound = true;
                }
            } else {
                printf("%04X\n", FATArray[i]);
                break;
            }
        }
    }
    
    printf("\n\n");
}

// Task 4
DirectoryEntry** listFiles(int fd, char *filename, BootSector *bootSector, size_t bootSectorSize) {
    ssize_t bytesRead;
    size_t numOfEntries = bootSector->BPB_RootEntCnt;
    off_t rootDIROffset = (bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16)) * bootSector->BPB_BytsPerSec;
    DirectoryEntry **directories = malloc(bootSector->BPB_RootEntCnt * sizeof(DirectoryEntry *));

    for (size_t i = 0; i < numOfEntries; i++)
    {
        DirectoryEntry *entry = malloc(sizeof(DirectoryEntry));
        bytesRead = readBytes(fd, filename, rootDIROffset + (i*sizeof(DirectoryEntry)), entry, sizeof(DirectoryEntry));
        directories[i] = entry;
    }

    // Formatted print into columns
    for (size_t i = 0; i < numOfEntries; i++) {
        if ((directories[i]->DIR_Name[7] != 0x00 && directories[i]->DIR_Name[7] != 0xE5) && directories[i]->DIR_Attr != 0x000F) {
            // Entry ID
            printf("%d", i);
            // File Name
            printf("%20s", directories[i]->DIR_Name + '\0');

            // Date and Time
            printf("           %02d-%02d-%02d %02d:%02d:%02d",
                (((directories[i]->DIR_WrtDate >> 9) & 0x7F) + 1980),
                ((directories[i]->DIR_WrtDate >> 5) & 0xF),
                (directories[i]->DIR_WrtDate & 0x1F),
                ((directories[i]->DIR_WrtTime >> 11) & 0x1F),
                ((directories[i]->DIR_WrtTime >> 5) & 0x3F),
                (directories[i]->DIR_WrtTime & 0x1F));
            
            // Attributes
            printf("%15c%c%c%c%c%c",
                (directories[i]->DIR_Attr & 0x20) ? 'A' : '-',
                (directories[i]->DIR_Attr & 0x10) ? 'D' : '-',
                (directories[i]->DIR_Attr & 0x08) ? 'V' : '-',
                (directories[i]->DIR_Attr & 0x04) ? 'S' : '-',
                (directories[i]->DIR_Attr & 0x02) ? 'H' : '-',
                (directories[i]->DIR_Attr & 0x01) ? 'R' : '-');
            
            // CLuster Info
            u_int32_t fullFstCluster = (((u_int32_t)directories[i]->DIR_FstClusHI << 16) +  directories[i]->DIR_FstClusLO);
            printf("              %08X", fullFstCluster);
        
            // File Length
            printf("%18d bytes\n", directories[i]->DIR_FileSize);
        }
    }
    printf("\n");
    return directories;
}

// Task 5
void openFile(int fd, char *filename, BootSector *bootSector, size_t bootSectorSize, DirectoryEntry **pDirectories) {
    int chosenFileID;
    DirectoryEntry chosenFile;
    off_t chosenFileOffset;
    ssize_t chosenFileSize;
    u_int32_t startingCluster;
    ssize_t bytesRead;
    
    // Get chosen file ID from user
    printf("Please enter the file to display: ");
    scanf("%d", &chosenFileID);

    // Copy chosen file into local struct instance and grab the file size and starting cluster
    memcpy(&chosenFile, pDirectories[chosenFileID], sizeof(DirectoryEntry));
    chosenFileSize = chosenFile.DIR_FileSize;
    startingCluster = (((u_int32_t)chosenFile.DIR_FstClusHI << 16) +  chosenFile.DIR_FstClusLO);

    // Calculate the file's offset in the image
    chosenFileOffset = (bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16)) * bootSector->BPB_BytsPerSec;
    chosenFileOffset += ((startingCluster + 2 + (2 * sizeof(u_int16_t))) * (bootSector->BPB_SecPerClus * bootSector->BPB_BytsPerSec));

    // Print the file contents until end of file
    char buffer[chosenFileSize];
    bytesRead = readBytes(fd, filename, chosenFileOffset, buffer, chosenFileSize);
    printf("File '%s' contains:\n%s",chosenFile.DIR_Name, buffer);
}

int main() {
    int fd;                                         // The file descriptor
    char filename[] = "fat16.img";                  // The fat16 image filename
    BootSector bootSector;                          // Struct of BootSector field
    size_t bootSectorSize = sizeof(bootSector);     // Size of BootSector in bytes
    DirectoryEntry** pDirectories;                  // Pointer to the DirectoryEntry pointer array (to access the root dir in other functions)


    // Task 2
    printf("--- Task 2 ---\n");
    printFields(fd, filename, &bootSector, bootSectorSize);         // Print all fields of the boot sector


    // Task 3
    printf("--- Task 3 ---\n");
    produceClusters(fd, filename, &bootSector, bootSectorSize);     // Load a copy of first FAT into memory, and print an ordered list of clusters


    // Task 4
    printf("--- Task 4 ---\n");
    pDirectories = listFiles(fd, filename, &bootSector, bootSectorSize);           // Output a list of files in the root directory


    // Task 5
    printf("--- Task 5 ---\n");
    openFile(fd, filename, &bootSector, bootSectorSize, pDirectories);            // Output the contents of a chosen file from a directory


}
