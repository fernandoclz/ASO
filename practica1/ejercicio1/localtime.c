#include <sys/types.h>
#include <time.h>
#include <stdio.h>

//consultar time(2), localtime(3), strftime(3)

int main()
{


/* Obtencion del tiempo actual (time_t) */
   time_t now;
   now = time(NULL);
   struct tm *info;
   char buffer[80];
/* Obtencion de la estructura  (struct tm) */
    
/* Formatear la fecha en una cadena con el patron ejemplo
   "Hoy es Viernes 11:40" */
    info = localtime(&now);
    strftime(buffer, 80, "Bienvenido, Hoy es %A, %H:%M del año %Y", info);
    
/* Mostrar la cadena resultante */    
    printf("%s\n", buffer);
    
    return 0;
}    
