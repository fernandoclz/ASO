#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int ancho, alto;
    unsigned char *datos;  // Array RGB (3 * ancho * alto)
} ImagenPPM;

void liberar_ppm(ImagenPPM *img) {
    free(img->datos);
    free(img);
}

ImagenPPM *leer_ppm(const char *nombre_fichero) {
    FILE *fichero = fopen(nombre_fichero, "r");
    if (!fichero) {
        perror("Error abriendo el fichero");
        return NULL;
    }

    char tipo[3];
    int ancho, alto, max_val;

    // Leer encabezado y verificar éxito
    if (fscanf(fichero, "%2s", tipo) != 1 || tipo[0] != 'P' || tipo[1] != '3') {
        fprintf(stderr, "Formato PPM no soportado o fichero corrupto\n");
        fclose(fichero);
        return NULL;
    }

    if (fscanf(fichero, "%d %d %d", &ancho, &alto, &max_val) != 3 || max_val != 255) {
        fprintf(stderr, "Error leyendo dimensiones o formato incorrecto\n");
        fclose(fichero);
        return NULL;
    }

    // Reservar memoria para la imagen
    ImagenPPM *img = (ImagenPPM *)malloc(sizeof(ImagenPPM));
    if (!img) {
        perror("Error al reservar memoria");
        fclose(fichero);
        return NULL;
    }

    img->ancho = ancho;
    img->alto = alto;
    img->datos = (unsigned char *)malloc(3 * ancho * alto);
    if (!img->datos) {
        perror("Error al reservar memoria para píxeles");
        free(img);
        fclose(fichero);
        return NULL;
    }

    // Leer píxeles asegurando que se leen correctamente
    for (int i = 0; i < 3 * ancho * alto; i++) {
        if (fscanf(fichero, "%hhu", &img->datos[i]) != 1) {
            fprintf(stderr, "Error al leer datos de píxeles\n");
            free(img->datos);
            free(img);
            fclose(fichero);
            return NULL;
        }
    }

    fclose(fichero);
    return img;
}

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("No hay suficientes arg, para %s", argv[0]);
        return 1;
    }
    const char *entrada = argv[1];
    const int iteraciones = atoi(argv[2]);

    long long medidas[iteraciones];
    for(int i = 0; i < iteraciones; i++){
        //iniciar medida
        struct timespec start, end;
        long long medida;

        clock_gettime(CLOCK_MONOTONIC, &start);
        ImagenPPM *img = leer_ppm(entrada);
        if(!img){
            printf("Error al leer la imagen");
            return 0;
        }
        //terminar medida
        clock_gettime(CLOCK_MONOTONIC, &end);
        //guardar medida
        medida = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
        medidas[i] = medida;
        liberar_ppm(img);
    }

    long long min = medidas[0];
    long long max = medidas[0];
    long long sum = 0;
    for(int i = 0; i < iteraciones; i++){
        if(medidas[i] < min) min = medidas[i];
        if(medidas[i] > max) max = medidas[i];
        sum += medidas[i];
    }
    long long mid = sum / iteraciones;
    printf("Tiempo medio: %lld ns\n", mid);
    printf("Tiempo maximo: %lld ns\n", max);
    printf("Tiempo minimo: %lld ns\n", min);
return 0;
}
