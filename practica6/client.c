#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define SERVER_QUEUE_NAME "/file_server_q"
#define MAX_MSG_SIZE 8192

typedef struct
{
    char filename[256];
    char client_queue_name[64];
} client_request_t;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Crear cola privada para respuestas
    char client_q_name[64];
    snprintf(client_q_name, sizeof(client_q_name), "/client_q_%d", getpid());

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mqd_t client_q = mq_open(client_q_name, O_CREAT | O_RDONLY, 0666, &attr);
    if (client_q == (mqd_t)-1)
    {
        perror("mq_open client");
        exit(EXIT_FAILURE);
    }

    // Preparar solicitud
    client_request_t request;
    strncpy(request.filename, argv[1], sizeof(request.filename));
    strncpy(request.client_queue_name, client_q_name, sizeof(request.client_queue_name));

    // Enviar solicitud al servidor
    mqd_t server_q = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if (server_q == (mqd_t)-1)
    {
        perror("mq_open server");
        mq_close(client_q);
        mq_unlink(client_q_name);
        exit(EXIT_FAILURE);
    }
    printf("Enviando solicitud al servidor: %s\n", request.filename);
    if (mq_send(server_q, (char *)&request, sizeof(request), 0) == -1)
    {
        perror("mq_send");
        mq_close(server_q);
        mq_close(client_q);
        mq_unlink(client_q_name);
        exit(EXIT_FAILURE);
    }

    mq_close(server_q);

    // Recibir respuesta
    char buffer[MAX_MSG_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = mq_receive(client_q, buffer, sizeof(buffer), NULL)) > 0)
    {
        if (bytes_read == -1)
        {
            perror("mq_receive");
            break;
        }

        if (strncmp(buffer, "ERROR:", 6) == 0)
        {
            fprintf(stderr, "%.*s\n", (int)bytes_read, buffer);
            break;
        }
        if (strncmp(buffer, "END", 3) == 0)
        {
            break;
        }

        printf("%.*s", (int)bytes_read, buffer);
    }
    printf("\n");
    // Limpieza
    mq_close(client_q);
    mq_unlink(client_q_name);

    return EXIT_SUCCESS;
}