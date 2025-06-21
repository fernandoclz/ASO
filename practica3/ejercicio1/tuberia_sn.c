#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char *argv[]){
    if(argc != 5){
        perror("Argumentos insuficientes\n");
        exit(1);
    }
    
    int pipefd[2]; //0 es lectura, 1 es escritura
    pid_t pid;

    if(pipe(pipefd) == -1){
        perror("pipe");
        exit(1);
    }

    pid = fork();
    if(pid < 0){
        perror("fork");
        exit(1);
    }
    if(pid >0){
        close(pipefd[0]);
        if(dup2(pipefd[1], STDOUT_FILENO) == -1){
            perror("dup2");
            exit(1);
        }
        close(pipefd[1]);
        execlp(argv[1], argv[1], argv[2], NULL);
        perror("execlp");
        exit(1);
    }
    else{
        close(pipefd[1]);
        if(dup2(pipefd[0], STDIN_FILENO) == -1){
            perror("dup2");
            exit(1);
        }
        close(pipefd[0]);
        execlp(argv[3], argv[3], argv[4], NULL);
        perror("execlp");
        exit(1);
    }
    wait(NULL);
    return 0;
}