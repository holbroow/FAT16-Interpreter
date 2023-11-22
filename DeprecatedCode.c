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