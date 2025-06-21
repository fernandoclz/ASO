#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppm.h"
#include <dlfcn.h>

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

    void *manejador;
    void (*aplicar_filtro)(ImagenPPM *);
    dlerror(); // Reiniciar errores
    // Aplicar filtro según el argumento
    if (strcmp(filtro, "gris") == 0) {
        manejador = dlopen("./libfiltro_gris.so", RTLD_LAZY);
        if (!manejador) {
            fprintf(stderr, "Error al abrir la biblioteca: %s\n", dlerror());
            liberar_ppm(img);
            return 1;
        }
        aplicar_filtro = dlsym(manejador, "filtro_gris");
        if (!aplicar_filtro) {
            fprintf(stderr, "Error al cargar el símbolo: %s\n", dlerror());
            liberar_ppm(img);
            dlclose(manejador);
            return 1;
        }
    } else if (strcmp(filtro, "sepia") == 0) {
        manejador = dlopen("./libfiltro_sepia.so", RTLD_LAZY);
        if (!manejador) {
            fprintf(stderr, "Error al abrir la biblioteca: %s\n", dlerror());
            liberar_ppm(img);
            return 1;
        }
        aplicar_filtro = dlsym(manejador, "filtro_sepia");
        if (!aplicar_filtro) {
            fprintf(stderr, "Error al cargar el símbolo: %s\n", dlerror());
            liberar_ppm(img);
            dlclose(manejador);
            return 1;
        }
    } else {
        printf("Filtro no válido: %s\n", filtro);
        printf("Filtros disponibles: gris, sepia\n");
        liberar_ppm(img);
        return 1;
    }

    aplicar_filtro(img);
    dlclose(manejador);
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
