#include "filtro_sepia.h"
void filtro_sepia(ImagenPPM *img) {
    for (int i = 0; i < img->ancho * img->alto * 3; i += 3) {
        unsigned char r = img->datos[i];
        unsigned char g = img->datos[i + 1];
        unsigned char b = img->datos[i + 2];

        // Aplicar la transformación sepia
        int nuevo_r = (int)((r * 0.393) + (g * 0.769) + (b * 0.189));
        int nuevo_g = (int)((r * 0.349) + (g * 0.686) + (b * 0.168));
        int nuevo_b = (int)((r * 0.272) + (g * 0.534) + (b * 0.131));

        // Asegurar que los valores estén dentro del rango [0,255]
        img->datos[i] = (nuevo_r > 255) ? 255 : (nuevo_r < 0) ? 0 : nuevo_r;
        img->datos[i + 1] = (nuevo_g > 255) ? 255 : (nuevo_g < 0) ? 0 : nuevo_g;
        img->datos[i + 2] = (nuevo_b > 255) ? 255 : (nuevo_b < 0) ? 0 : nuevo_b;
    }
}
