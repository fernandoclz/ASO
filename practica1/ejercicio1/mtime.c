#include<stdio.h>
#include<time.h>
// includes
int main(){
  

    char *shoy, *s;

    // consular la fecha actual
    time_t now;
    now = time(NULL);
    shoy= ctime(&now);
    printf("Hoy es: %s",shoy);
 

    // calcular que fecha fue hace 10 dias
    
    now -= 864000;
    s = ctime(&now);
    printf("Hace 10 dias fue: %s",s);    
    
    return 0;
}    

//ctime define su propia memoria estática en la funcion
// y puede dar errores al ejecutar ctime en varios hilos a 
// la vez. No hace falta liberar el puntero.
//existe ctime_r donde le das el buffer y evitar problemas
// de concurrencia en la escritura.