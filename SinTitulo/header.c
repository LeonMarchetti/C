#include "header.h"

// Librerías ==========================================================
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Constantes =========================================================

// Funciones ==========================================================
void* hilo_terminar(void* arg)
{
    pthread_detach(pthread_self());
    getchar();
    exit(0);
    return NULL;
}

pthread_t iniciar_hilo_terminar()
{
    pthread_t id_hilo;
    pthread_create(&id_hilo, NULL, hilo_terminar, NULL);
    return id_hilo;
}

char* leer_archivo(FILE* archivo)
{
    // Tamaño de archivo:
    fseek(archivo, 0, SEEK_END);
    long longitud = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);

    // Asignar espacio para el string:
    char* contenido = malloc(longitud);
    if (!contenido)
    {
        printf("No se pudo asignar memoria para el texto.\n");
        return NULL;
    }

    // Leer contenido del archivo:
    int leido = fread(contenido, 1, longitud, archivo);
    contenido[leido] = 0;
    fclose(archivo);
    return contenido;
}
