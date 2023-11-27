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
ssize_t readBytes(int fd, char *filename, off_t offset, char *buffer, ssize_t bytesToRead) {
    fd = open(filename, O_RDONLY);        // Open file
    lseek(fd, offset, SEEK_SET);          // Seek to the byte position for the bootsector
    return read(fd, buffer, bytesToRead); // Read the file from start the the end of the boot sector
    close(fd);                            // Close file
}

// RETURN CLUSTER AT OFFSET
u_int16_t getCluster(int fd, char *filename, off_t offset) {
    ssize_t bytesRead;
    u_int16_t cluster;
    bytesRead = readBytes(fd, filename, offset, &cluster, sizeof(u_int16_t));
    return cluster;
}


// TASK 2
void printFields(int fd, char *filename, BootSector *bootSector, size_t bootSectorSize) {
    off_t offset = 0;                        // How many bytes from the start of the file to begin
    size_t bytesToRead = sizeof(BootSector); // How many bytes to be read from the file
    ssize_t bytesRead;                       // How many bytes were actually read from the file

    bytesRead = readBytes(fd, filename, offset, bootSector, bytesToRead); // Runs readBytes functions to open the file, seek to the bytes, and read them to the bootSector struct directly

    // Print bootsector fields (debug)
    printf("BS_jmpBoot: ");
    for (size_t i = 0; i < sizeof(bootSector->BS_jmpBoot); i++)
    {
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

// TASK 3
void produceClusters(int fd, char *filename, BootSector *bootSector, size_t bootSectorSize) {
    ssize_t bytesRead;                                                                // How many bytes were actually read from the file
    off_t initialFATOffset = bootSector->BPB_RsvdSecCnt * bootSector->BPB_BytsPerSec; // The offset at which we will start reading (the position of the first fat cluster)
    u_int16_t FATArray[bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec / 2];     // Array for the first FAT
    u_int16_t previousCluster;
    u_int16_t retrievedCluster;
    bool firstFound = false;
    int chosenClusterIndex;

    printf("Please choose a cluster to read from: ");
    scanf("%d", &chosenClusterIndex);

    bytesRead = readBytes(fd, filename, initialFATOffset, FATArray, bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec);z

    printf("First cluster list: ");
    while(previousCluster < 0xFFF8) {
        if (!firstFound) {
            retrievedCluster = FATArray[0];
            printf("%04X - ", retrievedCluster);
            previousCluster = retrievedCluster;
            firstFound = true;
        } else {
            retrievedCluster = FATArray[previousCluster];
            printf("%04X - ", retrievedCluster);
            previousCluster = retrievedCluster;
        }
    }

    printf("\n\n");
}

// TASK 4 (need to expand for Task 6)
DirectoryEntry **listFiles(int fd, char *filename, BootSector *bootSector, size_t bootSectorSize, off_t dirOffset) {
    ssize_t bytesRead;
    size_t numOfEntries = bootSector->BPB_RootEntCnt;
    DirectoryEntry **entries = malloc(bootSector->BPB_RootEntCnt * sizeof(DirectoryEntry *));

    for (size_t i = 0; i < numOfEntries; i++) {
        DirectoryEntry *entry = malloc(sizeof(DirectoryEntry));
        bytesRead = readBytes(fd, filename, dirOffset + (i * sizeof(DirectoryEntry)), entry, sizeof(DirectoryEntry));
        entries[i] = entry;
    }

    // Formatted print into columns
    for (size_t i = 0; i < numOfEntries; i++) {
        if ((entries[i]->DIR_Name[11] != 0x00 && entries[i]->DIR_Name[11] != 0xE5)) {
            if (entries[i]->DIR_Attr == 0x000F) { // LONG FILENAME
            
            }
            else if (entries[i]->DIR_Attr != 0x000F) { // SHORT FILENAME
                // Entry ID
                printf("%d", i);

                // File Name
                printf("%20s", entries[i]->DIR_Name + '\0');

                // Date and Time
                printf("           %02d-%02d-%02d %02d:%02d:%02d",
                       (((entries[i]->DIR_WrtDate >> 9) & 0x7F) + 1980),
                       ((entries[i]->DIR_WrtDate >> 5) & 0xF),
                       (entries[i]->DIR_WrtDate & 0x1F),
                       ((entries[i]->DIR_WrtTime >> 11) & 0x1F),
                       ((entries[i]->DIR_WrtTime >> 5) & 0x3F),
                       (entries[i]->DIR_WrtTime & 0x1F));

                // Attributes
                printf("%15c%c%c%c%c%c",
                       (entries[i]->DIR_Attr & 0x20) ? 'A' : '-',
                       (entries[i]->DIR_Attr & 0x10) ? 'D' : '-',
                       (entries[i]->DIR_Attr & 0x08) ? 'V' : '-',
                       (entries[i]->DIR_Attr & 0x04) ? 'S' : '-',
                       (entries[i]->DIR_Attr & 0x02) ? 'H' : '-',
                       (entries[i]->DIR_Attr & 0x01) ? 'R' : '-');

                // CLuster Info
                u_int32_t fullFstCluster = (((u_int32_t)entries[i]->DIR_FstClusHI << 16) + entries[i]->DIR_FstClusLO);
                printf("              %08X", fullFstCluster);

                // File Length
                printf("%18d bytes\n", entries[i]->DIR_FileSize);
            }
        }
    }
    printf("\n");
    return entries;
}

// TASK 5
void openFile(int fd, char *filename, BootSector *bootSector, size_t bootSectorSize, DirectoryEntry **pEntries) {
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
    memcpy(&chosenFile, pEntries[chosenFileID], sizeof(DirectoryEntry));
    chosenFileSize = chosenFile.DIR_FileSize;
    startingCluster = (((u_int32_t)chosenFile.DIR_FstClusHI << 16) + chosenFile.DIR_FstClusLO);

    // Calculate the file's offset in the image
    chosenFileOffset = (bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16)) * bootSector->BPB_BytsPerSec;

    if (chosenFile.DIR_Attr == 0x10) {
        // handle printing the selected directory

    } else {
        chosenFileOffset += ((startingCluster + 2 + (2 * sizeof(u_int16_t))) * (bootSector->BPB_SecPerClus * bootSector->BPB_BytsPerSec));

        // Print the file contents until end of file
        char buffer[chosenFileSize];
        bytesRead = readBytes(fd, filename, chosenFileOffset, buffer, chosenFileSize);
        printf("File '%s' contains:\n%s", chosenFile.DIR_Name, buffer);
    }
}



// MAIN FUNCTION
int main() {
    int fd;                                     // The file descriptor
    char filename[] = "fat16.img";              // The fat16 image filename
    BootSector bootSector;                      // Struct of BootSector field
    size_t bootSectorSize = sizeof(bootSector); // Size of BootSector in bytes
    DirectoryEntry **pEntries;                  // Pointer to the DirectoryEntry pointer array (to access the root dir in other functions)
    off_t dirOffset;                            // Offset to read the root directory (initial call of listFIles functon)

    // Task 2
    printf("--- Task 2 ---\n");
    printFields(fd, filename, &bootSector, bootSectorSize); // Print all fields of the boot sector

    // Task 3
    printf("--- Task 3 ---\n");
    produceClusters(fd, filename, &bootSector, bootSectorSize); // Load a copy of first FAT into memory, and print an ordered list of clusters

    // Task 4
    printf("--- Task 4 ---\n");
    dirOffset = (bootSector.BPB_RsvdSecCnt + (bootSector.BPB_NumFATs * bootSector.BPB_FATSz16)) * bootSector.BPB_BytsPerSec;
    pEntries = listFiles(fd, filename, &bootSector, bootSectorSize, dirOffset); // Output a list of files in the root directory

    // Task 5
    printf("--- Task 5 ---\n");
    openFile(fd, filename, &bootSector, bootSectorSize, pEntries); // Output the contents of a chosen file from a directory


}
