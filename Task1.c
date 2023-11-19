#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

ssize_t readBytes(int fd, off_t offset, char *buffer, ssize_t bytesToRead) {
    fd = open("fat16.img", O_RDONLY);               // opens file
    lseek(fd, offset, SEEK_SET);                    // seeks to the byte position in the file
    return read(fd, buffer, bytesToRead);      // reads the bytes at that position
}

int main() {
    int fd;
    off_t offset = 0;
    size_t bytesToRead = 3;
    ssize_t bytesRead;
    char buffer[100];

    bytesRead = readBytes(fd, offset, buffer, bytesToRead);

    // prints said bytes
    printf("Bytes read: \n");                           
    for (size_t i = 0; i < bytesRead; i++) {
        printf("%02X, ", (unsigned char)buffer[i]);     // print individual byte itself
    }
    printf("\n");

    // close file
    close(fd);
}
