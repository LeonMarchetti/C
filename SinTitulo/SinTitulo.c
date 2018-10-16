// Librerías ==========================================================
#include "header.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Constantes =========================================================
const char* ARCHIVO = "sintitulo.txt";

// Funciones ==========================================================
void prueba_leer_archivo();


int main(int argc, char **argv)
{
    iniciar_hilo_terminar();
    int i = 0;
    while (1) {
        printf("Ciclo %d\n", i++);
        sleep(1);
    }

    // prueba_leer_archivo();
}

void prueba_leer_archivo()
{
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
