/* gcc -o "./run/SinTitulo.exe" SinTitulo.c header.c
 * run\SinTitulo.exe
 */
#include "header.h"

#include <stdio.h>
#include <stdlib.h>
//~ #include <unistd.h>

const char* ARCHIVO = "sintitulo.txt";

int main(int argc, char **argv)
{
    /* iniciar_hilo_terminar();
    while (1) {
        sleep(1);
    }
    */

    // Leer archivo de texto:
    FILE* archivo = fopen(ARCHIVO, "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo\n");
        exit(1);
    }

    char* texto = leer_archivo(archivo);
    if (texto) {
        printf("%s\n", texto);
        free(texto);
    }
}
