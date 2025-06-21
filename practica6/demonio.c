#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <linux/limits.h>
#include <sys/syslog.h>
#include "demonio.h"

void close_fd()
{
    DIR *dir;
    struct dirent *d;
    char fd_path[PATH_MAX];
    char dir_path[PATH_MAX];
    int fd;

    sprintf(dir_path, "/proc/%d/fd", getpid());
    if ((dir = opendir(dir_path)) == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((d = readdir(dir)) != NULL)
    {
        if ((strcmp(d->d_name, ".") != 0) && (strcmp(d->d_name, "..")))
        {
            fd = atoi(d->d_name);
            if ((fd != STDOUT_FILENO) && (fd != STDERR_FILENO) && (fd != STDIN_FILENO))
            {
                if (close(fd) < 0)
                {
                    perror("close");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    closedir(dir);
}

void reset_signals()
{
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    for (int sig = 1; sig < _NSIG; sig++)
    {
        if ((sig != SIGKILL) && (sig != SIGSTOP) && (sig != 33) && (sig != 32))
        {
            if (sigaction(sig, &sa, NULL) < 0)
            {
                perror("sigaction");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void reset_mask()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);
}

void double_fork()
{
    pid_t pidh, pidn;
    int pipe_[2]; // 0 read, 1 write
    if (pipe(pipe_) < 0)
    {
        perror("Pipe");
        exit(EXIT_FAILURE);
    }
    if ((pidh = fork()) < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pidh == 0)
    {
        if (setsid() == -1)
        {
            perror("setsid");
            exit(EXIT_FAILURE);
        }
        if ((pidn = fork()) < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pidn == 0)
        {
            close(pipe_[0]);

            int confirmation = c_daemon();

            write(pipe_[1], &confirmation, sizeof(int));
            close(pipe_[1]);
            return;
        }
        else
        {
            close(pipe_[0]);
            close(pipe_[1]);
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        int n;
        close(pipe_[1]);

        int confirmation;
        if ((n = read(pipe_[0], &confirmation, sizeof(int))) == -1)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }
        close(pipe_[0]);

        if (confirmation < 0)
        {
            fprintf(stderr, "Error al demonizar\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
}

int c_daemon()
{   
    openlog("mi-demonio", LOG_PID | LOG_NDELAY, LOG_DAEMON);
    int nullfd = open("/dev/null", O_RDWR);
    if (nullfd < 0) {
        syslog(LOG_ERR, "No se pudo abrir /dev/null");
        return -1;
    }
    dup2(nullfd, STDIN_FILENO);
    dup2(nullfd, STDOUT_FILENO);
    dup2(nullfd, STDERR_FILENO);
    close(nullfd);

    umask(0);
    chdir("/");

    openlog("mi-demonio", LOG_PID | LOG_NDELAY, LOG_DAEMON);

    // Usa /tmp para desarrollo, cambia a /var/run para producción
    const char *pidfile = "/var/run/mi-demonio.pid";
    int pid_fd = open(pidfile, O_RDWR | O_CREAT, 0644);
    if (pid_fd < 0)
    {
        syslog(LOG_ERR, "No se pudo abrir el archivo PID (%s)", pidfile);
        return -1;
    }

    if (flock(pid_fd, LOCK_EX | LOCK_NB) < 0)
    {
        syslog(LOG_ERR, "El demonio ya está en ejecución");
        close(pid_fd);
        return -1;
    }

    char pid_str[16];
    snprintf(pid_str, sizeof(pid_str), "%d\n", getpid());
    if (ftruncate(pid_fd, 0) < 0 || write(pid_fd, pid_str, strlen(pid_str)) < 0) {
        syslog(LOG_ERR, "No se pudo escribir el PID");
        close(pid_fd);
        return -1;
    }

    // Renuncia a privilegios solo si es root, solo si aplica
    // if (getuid() == 0) {
    //     if (setgid(65534) != 0 || setuid(65534) != 0)
    //     {
    //         syslog(LOG_ERR, "No se pudo renunciar a privilegios");
    //         close(pid_fd);
    //         return -1;
    //     }
    // }

    syslog(LOG_INFO, "Demonio inicializado correctamente (PID %d)", getpid());
    return 0;
}

void demonizar()
{
    /*
        Cerrar todos los descriptores de fichero abiertos excepto stdin, stdout y stderr.
    */
    close_fd();
    /*
        Restablecer todos los manejadores de señales al manejador por defecto (SIG_DFL)
    */
    reset_signals();
    /*
        Restablecer la máscara de señales con sigprocmask(). 
    */
    reset_mask();
    /*
        Eliminar variables de entorno innecesarias para la operación del demonio.
     */
    clearenv();
    /*
        Double fork() trick.
    */
    double_fork();
    // PASO 7
    /*
        Conectar stdin, stdout y stderr al dispositivo /dev/null.
    /*
        Restablecer la máscara umask a 0 
    */
    /*
        Cambiar el directorio de trabajo actual al directorio raíz (/).
    /*
        Escribir el PID del demonio en el fichero PID asociado a este demonio. 
    */
    /*
        Renunciar a privilegios, si es posible y aplicable. 
    */
    /*
        Notificar al proceso original que la inicialización está completa.
    */
    // FIN
    /*
        El proceso original, una vez confirmando que la inicialización del demonio ha sido correcta puede finalizar.
    */
}