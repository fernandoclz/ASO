#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/inotify.h>
#include <signal.h>

#define MAXMSJ 256
#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

int main() {
    pid_t pid;
    char buffer[MAXMSJ];
    FILE *mailbox;
    char *pause_secs_env = getenv("PAUSASECS");

    int pause_secs = pause_secs_env ? atoi(pause_secs_env) : 1;
    if (pause_secs <= 0) {
        fprintf(stderr, "Error: variable de entorno PAUSASECS no válida\n");
        return 1;
    }

    pid = fork();
    if (pid == -1) {
        perror("Error al crear el proceso hijo");
        return 1;
    } else if (pid == 0) {
        // Proceso hijo
        int fd = inotify_init();
        if (fd < 0) {
            perror("Error al inicializar inotify");
            exit(1);
        }

        int wd = inotify_add_watch(fd, "mailbox", IN_MODIFY);
        if (wd == -1) {
            perror("Error al añadir watch a mailbox");
            close(fd);
            exit(1);
        }

        while (1) {
            char event_buf[EVENT_BUF_LEN];
            int length = read(fd, event_buf, EVENT_BUF_LEN);
            if (length < 0) {
                perror("Error al leer eventos de inotify");
                break;
            }

            struct inotify_event *event = (struct inotify_event *)event_buf;
            if (event->mask & IN_MODIFY) {
                mailbox = fopen("mailbox", "r");
                if (mailbox == NULL) {
                    perror("Error al abrir el buzón");
                    break;
                }
                if (fgets(buffer, MAXMSJ, mailbox) != NULL) {
                    printf("Mensaje recibido por el hijo: %s", buffer);
                }
                fclose(mailbox);
            }
        }

        inotify_rm_watch(fd, wd);
        close(fd);
    } else {
        // Proceso padre
        while (1) {
            printf("Proceso padre: Introduzca un mensaje (max %d caracteres): ", MAXMSJ);
            if (fgets(buffer, MAXMSJ, stdin) == NULL) {
                kill(pid, SIGTERM);
                printf("Finalizando proceso padre\n");
                break;
            }

            mailbox = fopen("mailbox", "w");
            if (mailbox == NULL) {
                perror("Error al abrir el buzón");
                kill(pid, SIGTERM);
                exit(1);
            }
            fputs(buffer, mailbox);
            fclose(mailbox);

            sleep(pause_secs);
        }
    }

    return 0;
}