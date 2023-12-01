// FAT 16 IMAGE INTERPRETER/INTERFACE IN C
// BY WILL HOLBROOK NOV 2023
// STUDENT ID: 38722798


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


// READ BYTES AT OFFSET (TASK 1)
void readBytes(char *filename, off_t offset, void *buffer, ssize_t bytesToRead) {
    int fd = open(filename, O_RDONLY);    // Open file
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }
    if (lseek(fd, offset, SEEK_SET) == -1) { // Seek to the byte position for the bootsector
        perror("Error seeking in file");
        close(fd);
        exit(1);
    }
    read(fd, buffer, bytesToRead);        // Read the file from start the the end of the boot sector
    close(fd);                            // Close file
}

// RETURN CLUSTER AT OFFSET (UNUSED)
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

// TASK 5
void openEntry(char *filename, BootSector *bootSector, size_t bootSectorSize, DirectoryEntry chosenEntry) {
    off_t initialFATOffset = bootSector->BPB_RsvdSecCnt * bootSector->BPB_BytsPerSec;   // THE OFFSET AT WHICH TO BEGIN READING (the position of the first fat cluster)
    ssize_t chosenEntrySize;                                                            // SIZE OF THE CHOSEN ENTRY IN BYTES
    u_int32_t startingCluster;                                                          // STARTING CLUSTER OF THE CHOSEN ENTRY
    u_int16_t previousCluster;                                                          // PREVIOUS CLUSTER TRAVERSED WHEN FINDING CLUSTER LIST
    u_int16_t FATArray[bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec / 2];       // ARRAY CONTAINING THE FAT (FIRST FAT)
    u_int16_t *clusterList = malloc(100 * sizeof(u_int16_t));;                          // CLUSTER LIST FOR THE CHOSEN FILE
    int i = 0;                                                                          // STARTING INDEX FOR FINDING CLUSTER LIST IN FAT
    char *entryData = malloc(chosenEntrySize);                                          // ARRAY FOR HOLDING DATA READ FROM CLUSTER IN LIST


    // STARTING CLUSTER IN THE FAT FOR THE FILE
    startingCluster = (((u_int32_t)chosenEntry.DIR_FstClusHI << 16) + chosenEntry.DIR_FstClusLO);   // STARTING CLUSTER OF CHOSEN FILE

    // READ ENTIRE FAT INTO MEMORY
    readBytes(filename, initialFATOffset, FATArray, bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec);
    
    // GET CLUSTER LIST FROM THE FILE STARTING CLUSTER
    clusterList[i] = startingCluster; // MAKE STARTING CLUSTER THE FIRST IN THE LIST
    previousCluster = startingCluster; // SETTING THE PREVIOUS CLUSTER TO THE CLUSTER JUST SAVED
    while(previousCluster < 0xFFF8 && previousCluster != 0x0000) {
        if (FATArray[previousCluster] < 0xFFF8) {
            i++;
            clusterList[i] = FATArray[previousCluster];
            printf("%04X\n", clusterList[i]);
            previousCluster = clusterList[i];
        }
        break;
    }

    // READ DATA FROM DATA USING THE CLUSTER LIST
    for (size_t i = 0; i < 4; i++) {
        int entryDataPos = (bootSector->BPB_RsvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz16)) * bootSector->BPB_BytsPerSec;
        entryDataPos += ((startingCluster + 2 + (2 * sizeof(u_int16_t))) * (bootSector->BPB_SecPerClus * bootSector->BPB_BytsPerSec));
        // READ THE CHOSEN CLUSTER CONTENTS
        readBytes(filename, entryDataPos, entryData, (bootSector->BPB_SecPerClus * bootSector->BPB_BytsPerSec));
    }

    // PRINT THE DATA READ (THE FILE IN FULL)
    printf("File '%s' contains:\n%s", chosenEntry.DIR_Name, entryData);
}

// TASK 4
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




// MAIN FUNCTION
int main() {
    char filename[] = "fat16.img";             // THE FAT16 IMAGE FILENAME IN DIRECTORY
    BootSector bootSector;                      // STRUCT OF BOOTSECTOR AND ITS FIELDS
    size_t bootSectorSize = sizeof(bootSector); // SIZE OF BOOTSECTOR IN BYTES
    off_t dirOffset;                            // OFFSET TO READ THE ROOT DIRECTORY (initial call of listDir functon)
    int userChoice;                             // USER'S CHOICE FOR FUNCTION TO BE EXECUTED

    printf("+---------------------------+\n");
    printf("|     FAT16 Interpreter     |\n");
    printf("|       Will Holbrook       |\n");
    printf("|       Written in C        |\n");
    printf("+---------------------------+\n");

    while(true) {
        printf("\n\n+----------------------------------------+\n");
        printf("| Tasks:                                 |\n");
        printf("|    1. Populate and Print BootSector    |\n");
        printf("|    2. Produce cluster list             |\n");
        printf("|    3. Directory viewer & accessor      |\n");
        printf("|                                        |\n");
        printf("+----------------------------------------+\n");

        printf("+----------------------------------+\n");
        printf("|   Please choose a task to run:   |\n");
        printf("+----------------------------------+\n");

        printf(">> ");
        scanf("%d", &userChoice);
        printf("\n");

        switch (userChoice) {
            case 1:
                // TASK 2
                printFields(filename, &bootSector, bootSectorSize); // PRINT REQUIRED FIELDS FROM THE BOOTSECTOR
                break;
            case 2:
                // TASK 3
                produceClusters(filename, &bootSector, bootSectorSize); // LOAD A COPY OF FIRST FAT INTO MEMORY, AND PRODUCE AN ORDERED LIST OF CLUSTERS
                break;
            case 3:
                // TASK 4+5
                dirOffset = (bootSector.BPB_RsvdSecCnt + (bootSector.BPB_NumFATs * bootSector.BPB_FATSz16)) * bootSector.BPB_BytsPerSec;
                listDir(filename, &bootSector, bootSectorSize, dirOffset); // OUTPUT THE LIST OF FILES IN THE ROOT (Recursively called in openEntry when a directory is chosen)
                break;
        }
    }
}
