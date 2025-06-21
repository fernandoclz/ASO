#include "filtro_gris.h"

void filtro_gris(ImagenPPM *img) {
    for (int i = 0; i < img->ancho * img->alto * 3; i += 3) {
        unsigned char gris = (img->datos[i] + img->datos[i + 1] + img->datos[i + 2]) / 3;
        img->datos[i] = img->datos[i + 1] = img->datos[i + 2] = gris;
    }
}

