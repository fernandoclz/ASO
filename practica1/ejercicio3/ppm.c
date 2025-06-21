#include <stdio.h>
#include <stdlib.h>
#include "ppm.h"

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

void guardar_ppm(const char *nombre_fichero, ImagenPPM *img) {
    FILE *fichero = fopen(nombre_fichero, "w");
    if (!fichero) {
        perror("Error al guardar el fichero");
        return;
    }

    fprintf(fichero, "P3\n%d %d\n255\n", img->ancho, img->alto);

    int caracteres_en_linea = 0;  // Contador de caracteres por línea
    const int max_linea = 70;     // Máximo de caracteres por línea 
    int valores_en_linea = 0;     // Controla cuántos valores imprimimos por línea

    for (int i = 0; i < 3 * img->ancho * img->alto; i++) {
        int caracteres_a_escribir = fprintf(fichero, "%d ", img->datos[i]);
        caracteres_en_linea += caracteres_a_escribir;
        valores_en_linea++;

        // Asegurar que haya exactamente 3 valores por píxel y líneas bien alineadas
        if (valores_en_linea % (img->ancho * 3) == 0) {
            fprintf(fichero, "\n"); // Nueva línea después de cada fila completa
            caracteres_en_linea = 0;
        } else if (caracteres_en_linea >= max_linea) {
            fprintf(fichero, "\n");
            caracteres_en_linea = 0;
        }
    }

    fclose(fichero);
}



void liberar_ppm(ImagenPPM *img) {
    free(img->datos);
    free(img);
}
