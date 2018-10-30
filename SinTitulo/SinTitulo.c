// Librerías ==========================================================
#include "header.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Constantes =========================================================
const char* ARCHIVO = "sintitulo.txt";

// Funciones ==========================================================
void prueba_leer_archivo();


int main(int argc, char **argv)
{
    //~ iniciar_hilo_terminar();
    //~ int i = 0;
    //~ while (1) {
        //~ printf("Ciclo %d\n", i++);
        //~ sleep(1);
    //~ }

    //~ prueba_leer_archivo();

    const char* tabla_1 = "public.\"Cliente\"";
    //~ char tabla[] = "public.\"Cliente\"";
    char tabla[strlen(tabla_1)];
    strcpy(tabla, tabla_1);

    char* schema = malloc(strlen(tabla));
    char* table  = malloc(strlen(tabla));

    int   i      = 0;
    char* token  = strtok(tabla, ".");
    while (token != NULL)
    {
        switch (i++)
        {
            case 0: strcpy(schema, token); break;
            case 1: strcpy(table, token);
        }
        token = strtok(NULL, ".");
    }

    printf("Esquema:\t%s\nTabla:  \t%s\n", schema, table);
    free(schema);
    free(table);
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
