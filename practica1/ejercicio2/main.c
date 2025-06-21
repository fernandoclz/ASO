#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppm.h"
#include "filtro_gris.h"
#include "filtro_sepia.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <imagen_entrada.ppm> <filtro>\n", argv[0]);
        printf("Filtros disponibles: gris, sepia\n");
        return 1;
    }

    const char *imagen_entrada = argv[1];
    const char *filtro = argv[2];

    // Leer imagen PPM
    ImagenPPM *img = leer_ppm(imagen_entrada);
    if (!img) {
        printf("Error al leer la imagen: %s\n", imagen_entrada);
        return 1;
    }

    // Aplicar filtro según el argumento
    if (strcmp(filtro, "gris") == 0) {
        filtro_gris(img);
    } else if (strcmp(filtro, "sepia") == 0) {
        filtro_sepia(img);
    } else {
        printf("Filtro no válido: %s\n", filtro);
        printf("Filtros disponibles: gris, sepia\n");
        liberar_ppm(img);
        return 1;
    }

    // Crear nombre de archivo de salida
    char imagen_salida[256];
    snprintf(imagen_salida, sizeof(imagen_salida), "%s_%s.ppm", imagen_entrada, filtro);

    // Guardar la imagen con el filtro aplicado
    guardar_ppm(imagen_salida, img);
    printf("Imagen procesada y guardada como: %s\n", imagen_salida);

    // Liberar memoria
    liberar_ppm(img);

    return 0;
}
