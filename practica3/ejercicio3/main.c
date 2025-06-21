#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 256
#define MAX_MSG 10

int main(void){
    mkfifo("padre-hijo", 0666);
    mkfifo("hijo-padre", 0666);

    int pid = fork();

    if(pid > 0){
        //Padre
        int tuberiaP = open("padre-hijo", O_WRONLY);
        int tuberiaH = open("hijo-padre", O_RDONLY);

        char buffer[BUFFER_SIZE];
        int i = 0;
        char response;

        while(i < MAX_MSG){
            printf("Introduce un mensaje: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            write(tuberiaP, buffer, strlen(buffer)+1);
            read(tuberiaH, &response, sizeof(char));
            if(response == 'q'){
                printf("Fin de comunicacion.\n");
                break;
            }
            i++;
        }

        close(tuberiaP);
        close(tuberiaH);
        unlink("padre-hijo");
        unlink("hijo-padre");
    }
    else if(pid == 0){
        //Hijo
        int tuberiaP = open("padre-hijo", O_RDONLY);
        int tuberiaH = open("hijo-padre", O_WRONLY);

        char buffer[BUFFER_SIZE];
        int msg = 0;

        while(1){
            ssize_t bytes = read(tuberiaP, buffer, BUFFER_SIZE);
            if(bytes == 0){
                continue;
            }
            for(int i = 0; i < bytes; i++){
                buffer[i] = toupper(buffer[i]);
            }
            printf("%s\n", buffer);
            sleep(1);
            msg++;
            if(msg == MAX_MSG){
                write(tuberiaH, "q", sizeof(char));
                break;
            }
            else{
                write(tuberiaH, "n", sizeof(char));
            }   

            if(msg == MAX_MSG){
                break;
            }
        }
        close(tuberiaP);
        close(tuberiaH);
    }
    else{
        printf("Error al crear el proceso hijo.\n");
    }
    return 0;
}