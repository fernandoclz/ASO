#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define NOOP 0
#define LINES 1
#define EXE 2

int isText(FILE *file){
    unsigned char c;
            int contador_binario = 0, contador_texto = 0;
            size_t i, max_bytes = 512;  // Leeremos hasta 512 bytes

            fseek(file, 0, SEEK_SET);

            for (i = 0; i < max_bytes && fread(&c, 1, 1, file) == 1; i++) {
                if (isprint(c) || isspace(c)) {
                    contador_texto++;
                } else {
                    contador_binario++;
                }
            }

            fseek(file, 0, SEEK_SET);
            return (contador_binario > contador_texto * 0.1) ? 0 : 1;
}
///////////////////////////////////////////////
void f(char *filename, int flag){
    struct stat fileStat;
    int contador = 0;
    if(lstat(filename, &fileStat) < 0){
        fprintf(stderr, "Error: no se pudo obtener información del archivo\n");
        exit(1);
    }
    if(S_ISREG(fileStat.st_mode)){
        if(flag == LINES){
            FILE *file = fopen(filename, "rb");
            if(file == NULL){
                printf("Error: no se pudo abrir el archivo\n");
                exit(1);
            }
            
            if (isText(file) == 1) {
                char buffer[1024];
                contador++;
                while(fgets(buffer, sizeof(buffer), file) != NULL){
                    for (int i = 0; buffer[i] != '\0'; i++) {
                        if (buffer[i] == '\n') {
                            contador++;
                        }
                    }
                }
            }
        }
        int tam = fileStat.st_size;
        if(tam > 1024){
            if(contador > 0){
                printf("%s (inodo %ld, %d Kib, %d lineas)\n", filename, fileStat.st_ino, tam/1024, contador);
            }
            else{
              printf("%s (inodo %ld, %d Kib)\n", filename, fileStat.st_ino, tam/1024);
            }
        }
        else{
            if(contador > 0){
                printf("%s (inodo %ld, %d bytes, %d lineas)\n", filename, fileStat.st_ino, tam, contador);
            }
            else{
              printf("%s (inodo %ld, %d bytes)\n", filename, fileStat.st_ino, tam);
            }
        }
    }
    else{
        if(S_ISLNK(fileStat.st_mode)){
        printf("Enlace simbolico\n");
        }
        else if(S_ISDIR(fileStat.st_mode)){
            printf("Directorio\n");
        }
        else{
            printf("Archivo especial\n");
        }
    }
}
///////////////////////////////////////////////
void d(char *filename, int flag){
    struct stat fileStat;
    int contador=0;
    if(stat(filename, &fileStat) < 0){
        fprintf(stderr, "Error: no se pudo obtener información\n");
        exit(1);
    }
    if(S_ISDIR(fileStat.st_mode) && access(filename, R_OK)==0 && access(filename, X_OK)==0){
        DIR *dir = opendir(filename);
        struct dirent *entry;
        if(dir == NULL){
            fprintf(stderr, "Error: no se pudo abrir el directorio\n");
            exit(1);
        }
        while((entry = readdir(dir)) != NULL){
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
                char path[1024];
                struct stat entry_stat;
                snprintf(path, sizeof(path),"%s/%s", filename, entry->d_name);
                if(stat(path, &entry_stat) < 0){
                    fprintf(stderr, "Error: no se pudo obtener información\n");
                    exit(1);
                }

                if(flag == LINES && access(path, R_OK)==0){
                    contador = 0;
                    FILE *file = fopen(path, "r");
                    contador++;
                    if(file == NULL){
                        printf("Error: no se pudo abrir el archivo\n");
                        exit(1);
                    }
                    if(isText(file) == 1){
                        char buffer[1024];
                        contador++;
                        while(fgets(buffer, sizeof(buffer), file) != NULL){
                            for (int i = 0; buffer[i] != '\0'; i++) {
                                if (buffer[i] == '\n') {
                                    contador++;
                                }
                            }
                        }
                    }
                    fclose(file);
                }
                if(S_ISREG(entry_stat.st_mode)){
                    if(flag == LINES && access(path, R_OK) == 0){
                        printf("%s, fichero regular, lineas %d\n", entry->d_name, contador);
                    }
                    else if(flag == EXE && access(path, X_OK) == 0){
                        printf("%s, fichero regular con permiso de ejecucion\n", entry->d_name);
                    }
                    else if(flag != EXE && flag != LINES){
                        printf("%s, fichero regular\n", entry->d_name);
                    }
                }
                else if(S_ISLNK(entry_stat.st_mode) && flag != EXE && flag != LINES){ 
                    printf("%s, enlace simbolico\n", entry->d_name);
                }
                else if(S_ISDIR(entry_stat.st_mode) && flag != EXE && flag != LINES){
                    printf("%s, directorio\n", entry->d_name);
                }
                else if(S_ISCHR(entry_stat.st_mode) && flag != EXE && flag != LINES){
                    printf("%s, archivo especial de caracteres\n", entry->d_name);
                }
                else if(S_ISBLK(entry_stat.st_mode) && flag != EXE && flag != LINES){
                    printf("%s, archivo especial de bloques\n", entry->d_name);
                }
                else if(S_ISFIFO(entry_stat.st_mode) && flag != EXE && flag != LINES){
                    printf("%s, tuberia con nombre\n", entry->d_name);
                }
                else if(S_ISSOCK(entry_stat.st_mode) && flag != EXE && flag != LINES){
                    printf("%s, socket\n", entry->d_name);
                }
            }

        }
        closedir(dir);
    }

}
///////////////////////////////////////////////
int main(int argc, char *argv[]) {
    if(argc < 3){
        printf("Argumentos insuficientes\n");
        exit(1);
    }
    int opcion;
    int flag = NOOP;
    char *filename = NULL;
    char *output = NULL;
    static struct option long_options[] = {
        {"file", required_argument, NULL, 'f'},
        {"directory", required_argument, NULL, 'd'},
        {"lines", no_argument, NULL, 'l'},
        {"exe", no_argument, NULL, 'x'},
        {"output", required_argument, NULL, 'o'},
        {0, 0, 0, 0}
    };
    while((opcion = getopt_long(argc, argv,"f:d:lxo:", long_options, NULL)) != -1){
        switch(opcion){
            case 'l':
                flag = LINES;
                break;
            case 'x':
                flag = EXE;
                break;
            case 'f':
                filename = optarg;
                f(filename, flag);
                break;
            case 'd':
                filename = optarg;
                d(filename, flag);
                break;    
            case 'o':
                output = optarg;

            default:
                fprintf(stderr, "Error: opción no válida\n");
                exit(1);
        }
    }
    
    return 0;
}