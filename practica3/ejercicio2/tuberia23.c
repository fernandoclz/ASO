#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


int main(void){

    fd_set rfds;

    mkfifo("tuberia2", 0666);
    mkfifo("tuberia1", 0666);

    int ftub1 = open("tuberia1", O_RDONLY | O_NONBLOCK);
    if(ftub1 == -1){
        perror("open");
        exit(1);
    }

    int ftub2 = open("tuberia2", O_RDONLY | O_NONBLOCK);
    if(ftub2 == -1){
        perror("open");
        close(ftub1);
        exit(1);
    }

    printf("Pulse CTRL+D para terminar\n");

    while(1){
        FD_ZERO(&rfds);
        FD_SET(ftub2, &rfds);
        FD_SET(ftub1, &rfds);
        FD_SET(STDIN_FILENO, &rfds);

        int maximo = (ftub1 > ftub2) ? ftub1 : ftub2;

        if(STDIN_FILENO > maximo){
            maximo = STDIN_FILENO;
        }

        int ret = select(maximo +1, &rfds, NULL, NULL, NULL);
        if(ret == -1){
            perror("select");
            close(ftub1);
            close(ftub2);
            exit(1);
        }

        if(FD_ISSET(STDIN_FILENO, &rfds)){
            char buffer[256];
            if(read(STDIN_FILENO, buffer, 256) == 0){
                printf("Fin de la entrada estandar\n");
                break;
            }
        }

        if(FD_ISSET(ftub1, &rfds)){
            char buffer[256];
            int leidos = read(ftub1, buffer, 256);
            if(leidos == -1){
                perror("read");
                close(ftub1);
                close(ftub2);
                exit(1);
            }
            else if(leidos == 0){

            }
            else{
                buffer[leidos] = '\0';
                printf("Tuberia 1: %s\n", buffer);  
            }
        }

        if(FD_ISSET(ftub2, &rfds)){
            char buffer[256];
            int leidos = read(ftub2, buffer, 256);
            if(leidos == -1){
                perror("read");
                close(ftub1);
                close(ftub2);
                exit(1);
            }
            else if(leidos == 0){

            }
            else{
                buffer[leidos] = '\0';
                printf("Tuberia 2: %s\n", buffer);  
            }
        }


    }

    close(ftub1);
    close(ftub2);
    unlink("tuberia1");
    unlink("tuberia2");

    return 0;
}   