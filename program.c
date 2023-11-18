#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct __attribute__((__packed__)) {
    u_int8_t BS_jmpBoot[ 3 ]; // x86 jump instr. to boot code
    u_int8_t BS_OEMName[ 8 ]; // What created the filesystem
    u_int16_t BPB_BytsPerSec; // Bytes per Sector
    u_int8_t BPB_SecPerClus; // Sectors per Cluster
    u_int16_t BPB_RsvdSecCnt; // Reserved Sector Count
    u_int8_t BPB_NumFATs; // Number of copies of FAT
    u_int16_t BPB_RootEntCnt; // FAT12/FAT16: size of root DIR
    u_int16_t BPB_TotSec16; // Sectors, may be 0, see below
    u_int8_t BPB_Media; // Media type, e.g. fixed
    u_int16_t BPB_FATSz16; // Sectors in FAT (FAT12 or FAT16)
    u_int16_t BPB_SecPerTrk; // Sectors per Track
    u_int16_t BPB_NumHeads; // Number of heads in disk
    u_int32_t BPB_HiddSec; // Hidden Sector count
    u_int32_t BPB_TotSec32; // Sectors if BPB_TotSec16 == 0
    u_int8_t BS_DrvNum; // 0 = floppy, 0x80 = hard disk
    u_int8_t BS_Reserved1; //
    u_int8_t BS_BootSig; // Should = 0x29
    u_int32_t BS_VolID; // 'Unique' ID for volume
    u_int8_t BS_VolLab[ 11 ]; // Non zero terminated string
    u_int8_t BS_FilSysType[ 8 ]; // e.g. 'FAT16 ' (Not 0 term.)
} BootSector;


int main() {
    int fd; // fat16 image
    off_t offset = 0; // how many bytes from the start of the file to begin
    size_t bytesToRead = 3; // how many bytes to be read from the file
    ssize_t bytesRead; // how many bytes were actually read
    char buffer[100]; // the character buffer for the read bytes


    // opens file
    fd = open("fat16.img", O_RDONLY);                   

    // seeks to the byte position in the file
    lseek(fd, offset, SEEK_SET);                        

    // reads the bytes at that position
    bytesRead = read(fd, buffer, bytesToRead);         

    // prints said bytes
    printf("Bytes read: \n");                           
    for (size_t i = 0; i < bytesRead; i++) {
        printf("%02X, ", (unsigned char)buffer[i]); // print individual byte itself
    }
    printf("\n");

    // close file
    close(fd);
}
