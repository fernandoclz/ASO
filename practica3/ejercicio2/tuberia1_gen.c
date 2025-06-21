#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


int main (int argc, char *argv[]){
    if(argc != 2){
        printf("Argumentos insuficientes\n");
        exit(1);
    }

    const char *tuberia = "./tuberia1";
    int fd = open(tuberia, O_WRONLY);
    if(fd == -1){
        perror("open");
        exit(1);
    }
    if(write(fd, argv[1], strlen(argv[1])) == -1){
        perror("write");
        close(fd);
        exit(1);
    }
    close(fd);
    return 0;
}