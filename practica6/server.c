#include "sfile.h"
#include <mqueue.h>
#include "demonio.h"
#include <sys/syslog.h>

// gcc -o server server.c demonio.c -Ilib -lpthread -lrt
// gcc -o client client.c -lrt 

void handle_error(const char *msg)
{
    syslog(LOG_ERR, msg);
    exit(EXIT_FAILURE);
}

static void serveRequest(union sigval sv)
{
    mqd_t server_q = *((mqd_t *)sv.sival_ptr);
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];
    unsigned int prio;
    ssize_t bytes_read;

    // Obtener atributos de la cola
    if (mq_getattr(server_q, &attr) == -1)
    {
        syslog(LOG_ERR, "mq_getattr failed");
        return;
    }

    // Leer mensaje
    bytes_read = mq_receive(server_q, buffer, attr.mq_msgsize, &prio);
    if (bytes_read == -1)
    {
        syslog(LOG_ERR, "mq_receive failed");
        return;
    }

    // Procesar solicitud en un nuevo thread
    client_request_t *request = (client_request_t *)buffer;
    syslog(LOG_INFO, "Received request for file: %s", request->filename);

    // Abrir archivo solicitado
    mqd_t client_q = mq_open(request->client_queue_name, O_WRONLY);
    if (client_q == (mqd_t)-1)
    {
        syslog(LOG_ERR, "Failed to open client queue %s: %s (errno: %d)",
               request->client_queue_name, strerror(errno), errno);
        return;
    }

    // Abrir archivo solicitado
    FILE *file = fopen(request->filename, "rb");
    if (!file)
    {
        char error_msg[] = "ERROR: File not found";
        if (mq_send(client_q, error_msg, strlen(error_msg), 0) == -1)
        {
            syslog(LOG_ERR, "Failed to send error message");
        }
        mq_close(client_q);
        return;
    }

    char file_buffer[MAX_MSG_SIZE];
    size_t bytes;
    while ((bytes = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0)
    {
        if (mq_send(client_q, file_buffer, bytes, 0) == -1)
        {
            syslog(LOG_ERR, "Failed to send file chunk");
            break;
        }
    }

    fclose(file);
    const char end_msg[] = "END";
    mq_send(client_q, end_msg, strlen(end_msg), 0);
    mq_close(client_q);

    // Volver a registrar la notificación
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = serveRequest;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = &server_q;

    if (mq_notify(server_q, &sev) == -1)
    {
        syslog(LOG_ERR, "mq_notify failed");
    }
    syslog(LOG_INFO, "Finished processing request for file: %s", request->filename);
}

int main(int argc, char *argv[])
{
    mq_unlink(SERVER_QUEUE_NAME); // Eliminar cola de mensajes si existe

    demonizar();

    openlog("file_server", LOG_PID, LOG_DAEMON);

    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE + 1];

    // Set up the server queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10; // Max messages in queue
    attr.mq_msgsize = sizeof(client_request_t);
    attr.mq_curmsgs = 0; // Current messages in queue

    // Crear o abrir la cola de mensajes
    mqd_t serverId = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if (serverId == (mqd_t)-1)
    {
        syslog(LOG_ERR, "mq_open");
        exit(EXIT_FAILURE);
    }

    // Register for message notification using mq_notify
    struct sigevent sev;

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = serveRequest;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = &serverId; /* Arg. to thread func. */
    if (mq_notify(serverId, &sev) == -1)
        handle_error("mq_notify");

    syslog(LOG_INFO, "Server is running...");
    while (1)
    {
        pause(); // Wait for notification
    }

    // If mq_receive() fails, remove server MQ and exit
    if (mq_close(serverId) == -1)
    {
        perror("mq_close");
        exit(EXIT_FAILURE);
    }

    if (mq_unlink(SERVER_QUEUE_NAME) == -1)
    {
        perror("mq_unlink");
        exit(EXIT_FAILURE);
    }
    closelog();
    exit(EXIT_SUCCESS);
}
