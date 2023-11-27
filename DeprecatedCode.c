    // clearBuffer(buffer);
    // for (int i=0; i<sizeof(buffer);i++) {
    //     printf("%d\n", buffer[i]);
    // }

    // void clearBuffer(char *buffer) {
    //     for (size_t i = 0; i < sizeof(&buffer); i++) {
    //         buffer[i] = 'a';
    //     }
        
    // }





    DirectoryEntry dir1;
    bytesRead = readBytes(fd, filename, rootDIROffset, &dir1, sizeof(DirectoryEntry));

    for (size_t j = 0; j < sizeof(dir1.DIR_Name); j++)
        {
            printf("%c", dir1.DIR_Name);

        }
        printf("\n");
        printf("DIR_Attr: %08X\n", dir1.DIR_Attr);
        printf("DIR_NTRes: %08X\n", dir1.DIR_NTRes);   
        printf("DIR_CrtTimeTenth: %08X\n", dir1.DIR_CrtTimeTenth);
        printf("DIR_CrtTime: %016X\n", dir1.DIR_CrtTime);
        printf("DIR_CrtDate: %016X\n", dir1.DIR_CrtDate);
        printf("DIR_LstAccDate: %016X\n", dir1.DIR_LstAccDate);
        printf("DIR_FstClusHI: %016X\n", dir1.DIR_FstClusHI);
        printf("DIR_WrtTime: %016X\n", dir1.DIR_WrtTime);
        printf("DIR_WrtDate: %016X\n", dir1.DIR_WrtDate);
        printf("DIR_FstClusLO: %016X\n", dir1.DIR_FstClusLO);
        printf("DIR_FileSize: %032X\n", dir1.DIR_FileSize);


    // DirectoryEntry directoryEntries[bootSector->BPB_RootEntCnt];
    // size_t dirsRead = 0;

    // for (size_t i = 0; i < bootSector->BPB_RootEntCnt; i++)
    // {
    //     DirectoryEntry newDirectoryEntry;
    //     bytesRead = readBytes(fd, filename, rootDIROffset + (dirsRead * 32), &newDirectoryEntry, sizeof(DirectoryEntry));
    //     if(directoryEntries[i].DIR_Name[11] == 0) {
    //         break;
    //     }
    //     directoryEntries[i] = newDirectoryEntry;
    //     dirsRead++;
    // }


    // for (size_t i = 0; i < sizeof(directoryEntries); i++)
    // {
    //     if(directoryEntries[i].DIR_Name[0] == 0) {
    //         break;
    //     }
    //     for (size_t j = 0; j < sizeof(directoryEntries[i].DIR_Name); j++)
    //     {
    //         printf("%c", directoryEntries[i].DIR_Name);

    //     }
    //     printf("\n");
    //     printf("DIR_Attr: %08X\n", directoryEntries[i].DIR_Attr);
    //     printf("DIR_NTRes: %08X\n", directoryEntries[i].DIR_NTRes);   
    //     printf("DIR_CrtTimeTenth: %08X\n", directoryEntries[i].DIR_CrtTimeTenth);
    //     printf("DIR_CrtTime: %016X\n", directoryEntries[i].DIR_CrtTime);
    //     printf("DIR_CrtDate: %016X\n", directoryEntries[i].DIR_CrtDate);
    //     printf("DIR_LstAccDate: %016X\n", directoryEntries[i].DIR_LstAccDate);
    //     printf("DIR_FstClusHI: %016X\n", directoryEntries[i].DIR_FstClusHI);
    //     printf("DIR_WrtTime: %016X\n", directoryEntries[i].DIR_WrtTime);
    //     printf("DIR_WrtDate: %016X\n", directoryEntries[i].DIR_WrtDate);
    //     printf("DIR_FstClusLO: %016X\n", directoryEntries[i].DIR_FstClusLO);
    //     printf("DIR_FileSize: %032X\n", directoryEntries[i].DIR_FileSize);
    // }


printf("%d\n", bootSector->BPB_RsvdSecCnt);
    // printf("%d\n", bootSector->BPB_NumFATs);
    // printf("%d\n", bootSector->BPB_FATSz16);
    // printf("%d\n", rootDIROffset);
    // printf("\n");
    


// Temporary Old Print Formatting (debug)
for (size_t i = 0; i < sizeof(directories); i++)
{
    if (directories[i]->DIR_Attr != 0x000F) { // make sure that ignored entries with all 0-3 bits set are not printed (they are still stored though which may be an issue)
        // File Name
        printf("DIR_Name: %s\n", directories[i]->DIR_Name);
        // File Attributes
        printf("DIR_Attr: ");
        printf("%c", (directories[i]->DIR_Attr & 0x20) ? 'A' : '-'); // Bit 5
        printf("%c", (directories[i]->DIR_Attr & 0x10) ? 'D' : '-'); // Bit 4
        printf("%c", (directories[i]->DIR_Attr & 0x08) ? 'V' : '-'); // Bit 3
        printf("%c", (directories[i]->DIR_Attr & 0x04) ? 'S' : '-'); // Bit 2
        printf("%c", (directories[i]->DIR_Attr & 0x02) ? 'H' : '-'); // Bit 1
        printf("%c", (directories[i]->DIR_Attr & 0x01) ? 'R' : '-'); // Bit 0
        printf("\n");
        // Cluster Info
        printf("DIR_FstClusLO: %04X\n", directories[i]->DIR_FstClusLO);
        printf("DIR_FstClusHI: %04X\n", directories[i]->DIR_FstClusHI);
        // Write Date and Time
        printf("DIR_WrtDate + DIR_WrtTime: %04d-%02d-%02d  %02d-%02d-%02d\n", (((directories[i]->DIR_WrtDate >> 9) & 0x7F) + 1980), ((directories[i]->DIR_WrtDate >> 5) & 0xF), (directories[i]->DIR_WrtDate & 0x1F), ((directories[i]->DIR_WrtTime >> 11) & 0x1F), ((directories[i]->DIR_WrtTime >> 5) & 0x3F), (directories[i]->DIR_WrtTime & 0x1F)); // Formatted to YYYY-MM-DD
        // File Size
        printf("DIR_FileSize: %d bytes\n", directories[i]->DIR_FileSize);
        printf("\n\n");
    }
}

// // Task 3
// void produceClusters(int fd, char *filename, BootSector *bootSector, size_t bootSectorSize) {
//     ssize_t bytesRead;                                                                // How many bytes were actually read from the file
//     off_t initialFATOffset = bootSector->BPB_RsvdSecCnt * bootSector->BPB_BytsPerSec; // The offset at which we will start reading (the position of the first fat cluster)
//     size_t clusterCount = 0;                                                          // Counter for the amount of clusters read from the file
//     u_int16_t FATArray[bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec / 2];     // Array for the first FAT
//     bool firstFound = false;

//     printf("Offset for first FAT byte: %d\n", initialFATOffset); // debug - Print the FAT offset to monitor the functions read start point
//     bytesRead = readBytes(fd, filename, initialFATOffset, FATArray, bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec);

//     printf("First cluster list: ");
    

//     // printf("First cluster list: ");
//     // for (size_t i = 0; i < bootSector->BPB_FATSz16 * bootSector->BPB_BytsPerSec / 2; i++) {
//     //     if (!firstFound && (FATArray[i] == 0000 || FATArray[i] >= 0xFFF8)) {
//     //         // ignore value
//     //     }
//     //     else {
//     //         if (FATArray[i] < 0xFFF8) {
//     //             printf("%04X -", FATArray[i]);
//     //             if (!firstFound) {
//     //                 firstFound = true;
//     //             }
//     //         }
//     //         else {
//     //             printf("%04X\n", FATArray[i]);
//     //             break;
//     //         }
//     //     }
//     // }

//     printf("\n\n");
// }


// Print u_int16_t values in binary (unused)
void printBinary16(int x) {
    for (int i = 0; i < 16; i++)
    {
        printf("%d", (x & 0x8000) >> 15);
        x <<= 1;
    }
}
// Print u_int8_t values in binary (unused)
void printBinary8(int x) {
    for (int i = 0; i < 8; i++)
    {
        printf("%d", (x & 0x8000) >> 15);
        x <<= 1;
    }
}