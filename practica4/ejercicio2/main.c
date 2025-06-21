#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXMSJ 256

volatile sig_atomic_t ready = 0;

void handler1(int signum){
    printf("Recibido SIGUSR1\n");
    ready = 1;
}
void handler2(int signum){
    printf("Recibido SIGUSR2\n");
    ready = 1;
}

int main (){
    pid_t pid;
    char buffer[MAXMSJ];
    FILE *mailbox;
    char *pause_secs_env = getenv("PAUSASECS");

    int pause_secs = pause_secs_env ? atoi(pause_secs_env) : 1;
    if(pause_secs <= 0){
        fprintf(stderr, "Error: variable de entorno PAUSASEC no valida\n");
        return 1;
    }

    struct sigaction sa_usr1, sa_usr2;
    sa_usr1.sa_handler = handler1;
    sa_usr2.sa_handler = handler2;
    sa_usr1.sa_flags = SA_RESTART;
    sa_usr2.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa_usr1, NULL);
    sigaction(SIGUSR2, &sa_usr2, NULL);

    pid = fork();
    if(pid == -1){
        perror("Error al crear el proceso hijo");
        return 1;
    }
    else if(pid == 0){
        // Proceso hijo
        while(1){
            pause();
            if(ready){
                ready = 0;
                mailbox = fopen("mailbox", "r");
                if(mailbox == NULL){
                    perror("Error al abrir el buzón");
                    exit(1);
                }
                if(fgets(buffer, MAXMSJ, mailbox) != NULL){
                    printf("Mensaje recibido por el hijo: %s", buffer);
                }
                fclose(mailbox);
                kill(getppid(), SIGUSR2);
            }
        }
    }
    else{
        // Proceso padre
        while(1){
            printf("Proceso padre: Introduzca un mensaje (max %d caracteres): ", MAXMSJ);
            if(fgets(buffer, MAXMSJ, stdin) == NULL){
                kill(pid, SIGTERM);
                printf("Finalizando proceso padre\n");
                break;
            }

            mailbox = fopen("mailbox", "w");
            if(mailbox == NULL){
                perror("Error al abrir el buzón");
                kill(pid, SIGTERM);
                exit(1);
            }
            fputs(buffer, mailbox);
            fclose(mailbox);

            kill(pid, SIGUSR1);

            pause();
            if(ready){
                ready = 0;
                sleep(pause_secs);
            }
        }
    }
    return 0;
}