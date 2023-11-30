// IMPORT REQUIRED LIBRARIES
#include "functions.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


// READ BYTES AT OFFSET
void readBytes(char *filename, off_t offset, void *buffer, ssize_t bytesToRead) {
    int fd = open(filename, O_RDONLY);        // Open file
    lseek(fd, offset, SEEK_SET);          // Seek to the byte position for the bootsector
    read(fd, buffer, bytesToRead);        // Read the file from start the the end of the boot sector
    close(fd);                            // Close file
}

// RETURN CLUSTER AT OFFSET
u_int16_t getCluster(char *filename, off_t offset) {
    u_int16_t cluster;
    readBytes(filename, offset, &cluster, sizeof(u_int16_t));
    return cluster;
}



// TASK 2
void printFields(char *filename, BootSector *bootSector, size_t bootSectorSize) {
    off_t offset = 0;                           // WHERE TO BEGIN IN THE IMAGE TO READ THE BOOT SECTOR
    size_t bytesToRead = sizeof(BootSector);    // HOW MANY BYTES TO BE READ FROM THE FILE

    // READ BOOTSECTOR INFO FROM FILE INTO STRUCT
    readBytes(filename, offset, bootSector, bytesToRead);

    // PRINT REQUIRED BOOTSECTOR FIELDS
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
void produceClusters(char *filename, BootSector *bootSector, size_t bootSectorSize) {
    off_t initialFATOffset = bootSector->BPB_RsvdSecCnt * bootSector->BPB_BytsPerSec;   // THE OFFSET AT WHICH TO BEGIN READING (the position of the first fat cluster)
    u_int16_t FATArray[bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec / 2];       // ARRAY CONTAINING THE FAT (FIRST FAT)
    u_int16_t previousCluster;                                                          // PREVIOUS CLUSTER RETRIEVED IN THE LIST
    u_int16_t retrievedCluster;                                                         // CURRENT CLUSTER RETRIEVED IN THE LIST
    bool firstFound = false;                                                            // HAS THE FIRST CLUSTER BEEN FOUND IN THE LIST YET
    int chosenClusterIndex;                                                             // CHOSEN CLUSTER INDEX TO START THE LIST FROM

    // USER CHOOSES A CLUSTER NUMBER TO BEGIN WITH
    printf("Please choose a cluster to read from: ");
    scanf("%d", &chosenClusterIndex);

    // READ ENTIRE FAT INTO MEMORY
    readBytes(filename, initialFATOffset, FATArray, bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec);

    //FIND AND PRINT CLUSTER LIST STARTING AT CHOICE
    printf("First cluster list: ");
    while(previousCluster < 0xFFF8) {
        if (!firstFound) {
            retrievedCluster = FATArray[chosenClusterIndex];
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
void listDir(char *filename, BootSector *bootSector, size_t bootSectorSize, off_t dirOffset) {
    size_t numOfEntries = bootSector->BPB_RootEntCnt;                                               // NUMBER OF POSSIBLE ENTRIES IN THE ROOT DIRECTORY
    DirectoryEntry **entries = malloc(bootSector->BPB_RootEntCnt * sizeof(DirectoryEntry));       // ARRAY OF POINTERS TO ENTRIES, READ FROM THE ROOT DIRECTORY POSITION
    int chosenFileID;                                                                               // ID FOR THE CHOSEN ENTRY TO SELECT AND OPEN
    DirectoryEntry chosenEntry;                                                                     // STRUCT TO HOLD THE SELECTED ENTRY

    // SAVE DIRECTORY ENTRIES INTO AN ARRAY
    for (size_t i = 0; i < numOfEntries; i++) {
        DirectoryEntry *entry = (DirectoryEntry*) malloc(sizeof(DirectoryEntry));
        readBytes(filename, dirOffset + (i * sizeof(DirectoryEntry)), entry, sizeof(DirectoryEntry));
        entries[i] = entry;
    }

    // PRINT DIRECTORY INTO COLUMNS
    for (size_t i = 0; i < bootSector->BPB_RootEntCnt; i++) {
        if (entries[i]->DIR_Name[0] == 0x00) {
            break;
        }
        if (entries[i]->DIR_Name[0] != 0xE5) {
            if (entries[i]->DIR_Attr != 0x000F) { // SHORT FILENAME
                // ENTRY ID
                printf("%-20ld", i);

                // FILENAME
                printf("%-30s", entries[i]->DIR_Name + '\0');

                // ATTRIBUTES
                printf("%c%c%c%c%c%c",
                        (entries[i]->DIR_Attr & 0x20) ? 'A' : '-',
                        (entries[i]->DIR_Attr & 0x10) ? 'D' : '-',
                        (entries[i]->DIR_Attr & 0x08) ? 'V' : '-',
                        (entries[i]->DIR_Attr & 0x04) ? 'S' : '-',
                        (entries[i]->DIR_Attr & 0x02) ? 'H' : '-',
                        (entries[i]->DIR_Attr & 0x01) ? 'R' : '-');

                // DATE AND TIME
                printf("%10d-%d-%d %d:%d:%d",
                        (((entries[i]->DIR_WrtDate >> 9) & 0x7F) + 1980),
                        ((entries[i]->DIR_WrtDate >> 5) & 0xF),
                        (entries[i]->DIR_WrtDate & 0x1F),
                        ((entries[i]->DIR_WrtTime >> 11) & 0x1F),
                        ((entries[i]->DIR_WrtTime >> 5) & 0x3F),
                        (entries[i]->DIR_WrtTime & 0x1F));

                // STARTING CLUSTER
                u_int32_t fullFstCluster = (((u_int32_t)entries[i]->DIR_FstClusHI << 16) + entries[i]->DIR_FstClusLO);
                printf("              %08X", fullFstCluster);

                // FILE SIZE/LENGTH
                printf("%18d bytes\n", entries[i]->DIR_FileSize);
            }
        }
    }
    printf("\n");

    // OPEN THE ENTRY CHOSEN BY THE USER
    printf("Please enter the file to display: ");
    scanf("%d", &chosenFileID);

    // COPY CHOSEN FILE INTO A STRUCT INSTANCE
    memcpy(&chosenEntry, entries[chosenFileID], sizeof(DirectoryEntry));

    // OPEN THE CHOSEN ENTRY, PASSING IT IN
    openEntry(filename, bootSector, bootSectorSize, chosenEntry);
}

// TASK 5
void openEntry(char *filename, BootSector *bootSector, size_t bootSectorSize, DirectoryEntry chosenEntry) {
    off_t chosenEntryOffset;            // OFFSET TO READ THE CHOSEN ENTRY FROM THE FILE
    ssize_t chosenEntrySize;            // SIZE OF THE CHOSEN ENTRY IN BYTES
    u_int32_t startingCluster;          // STARTING CLUSTER OF THE CHOSEN ENTRY

    chosenEntrySize = chosenEntry.DIR_FileSize;                                                     // FILE SIZE OF CHOSEN FILE
    startingCluster = (((u_int32_t)chosenEntry.DIR_FstClusHI << 16) + chosenEntry.DIR_FstClusLO);   // STARTING CLUSTER OF CHOSEN FILE

    // CALCULATE FILE OFFSET IN FAT IMAGE
    chosenEntryOffset = (bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16)) * bootSector->BPB_BytsPerSec;

    if (chosenEntry.DIR_Attr == 0x10) {             // DIRECTORY
        // PRINT THE CHOSEN DIRECTORY
        // issues: prints past the DIR
        chosenEntryOffset += ((startingCluster + 2 + (2 * sizeof(u_int16_t))) * (bootSector->BPB_SecPerClus * bootSector->BPB_BytsPerSec));
        listDir(filename, bootSector, bootSectorSize, chosenEntryOffset);

    } else if (chosenEntry.DIR_Attr == 0x20) {      // ARCHIVE
        // PRINT THE CHOSEN ARCHIVE
        // issues: prints past the DIR
        chosenEntryOffset += ((startingCluster + 2 + (2 * sizeof(u_int16_t))) * (bootSector->BPB_SecPerClus * bootSector->BPB_BytsPerSec));
        listDir(filename, bootSector, bootSectorSize, chosenEntryOffset);
        
        
    } else if (chosenEntry.DIR_Attr == 0x08) {      // VOLUME
        // PRINT ERROR MESSAGE AND RETURN TO ROOT
        printf("You have selected a volume entry. Returning to the root directory.\n\n");
        listDir(filename, bootSector, bootSectorSize, (bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16)) * bootSector->BPB_BytsPerSec);
        
    } else if (chosenEntry.DIR_Attr == 0x0F) {      // INVALID/HIDDEN (e.g. LONG FILENAME)
        // PRINT ERROR MESSAGE AND RETURN TO ROOT
        printf("You have selected a hidden/invalid entry. Returning to the root directory.\n\n");
        listDir(filename, bootSector, bootSectorSize, (bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16)) * bootSector->BPB_BytsPerSec);
    
    } else {                                        // REGULAR FILE
        chosenEntryOffset += ((startingCluster + 2 + (2 * sizeof(u_int16_t))) * (bootSector->BPB_SecPerClus * bootSector->BPB_BytsPerSec));
        // PRINT THE CHOSEN FILE CONTENTS
        char buffer[chosenEntrySize];
        readBytes(filename, chosenEntryOffset, buffer, chosenEntrySize);
        printf("File '%s' contains:\n%s", chosenEntry.DIR_Name, buffer);

    }
}