#include <stdio.h>
#include "functions.h"

// MAIN FUNCTION
int main() {
    char filename[] = "fat16.img";              // THE FAT16 IMAGE FILENAME IN DIRECTORY
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
                printFields(filename, &bootSector, bootSectorSize); // PRINT REQUIRED FIELDS FROM THE BOOTSECTOR
                break;
            case 2:
                produceClusters(filename, &bootSector, bootSectorSize); // LOAD A COPY OF FIRST FAT INTO MEMORY, AND PRODUCE AN ORDERED LIST OF CLUSTERS
                break;
            case 3:
                dirOffset = (bootSector.BPB_RsvdSecCnt + (bootSector.BPB_NumFATs * bootSector.BPB_FATSz16)) * bootSector.BPB_BytsPerSec;
                listDir(filename, &bootSector, bootSectorSize, dirOffset); // OUTPUT THE LIST OF FILES IN THE ROOT (Recursively called in openEntry when a directory is chosen)
                break;
        }
    }
}
