#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

time_t start_time;
volatile sig_atomic_t run = 1;

void handler(int signum) {
    // This function is called when a signal is received
    time_t end_time = time(NULL);
    printf("Signal %d received, %d segundos.\n", signum, end_time - start_time);
    run = 0;
    
}

int main(int argc, char *argv[]) {
    // Ignore SIGINT signal CTRL+C
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    if(sigaction(SIGINT, &sa, NULL) == -1){
        perror("sigaction SIGINT");
        return 1;
    }
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <signal_number>\n", argv[0]);
        return 1;
    }
    int interval = atoi(argv[1]);
    if(interval <= 0) {
        fprintf(stderr, "Interval must be a positive integer\n");
        return 1;
    }

    // Abrir una terminal 
    // y ejecutar el comando "kill <pid>"
    // PID del proceso -> ps aux | grep <nombre del programa>
    struct sigaction sa_term;
    sa_term.sa_handler = handler;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if(sigaction(SIGTERM, &sa_term, NULL) == -1){
        perror("sigaction SIGTERM");
        return 1;
    }
    
    start_time = time(NULL);
    
    while(run){
        printf("Procesando...\n");
        sleep(interval);
    }
    return 0;
}