#ifndef PPM_H
#define PPM_H

typedef struct {
    int ancho, alto;
    unsigned char *datos;  // Array RGB (3 * ancho * alto)
} ImagenPPM;

ImagenPPM *leer_ppm(const char *nombre_fichero);
void guardar_ppm(const char *nombre_fichero, ImagenPPM *img);
void liberar_ppm(ImagenPPM *img);

#endif
