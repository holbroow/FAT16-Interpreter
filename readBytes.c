#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd;
    off_t offset = 0;
    size_t bytesToRead = 3;
    ssize_t bytesRead;
    char buffer[100];


    // opens file
    fd = open("fat16.img", O_RDONLY);                   

    // seeks to the byte position in the file
    lseek(fd, offset, SEEK_SET);                        

    // reads the bytes at that position
    bytesRead = read(fd, buffer, bytesToRead);         

    // prints said bytes
    printf("Bytes read: \n");                           
    for (size_t i = 0; i < bytesRead; i++) {
        printf("%02X, ", (unsigned char)buffer[i]);     // print individual byte itself
    }
    printf("\n");

    // close file
    close(fd);
}
