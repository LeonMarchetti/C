// Librerías ==========================================================
#include <json.h>
#include <stdio.h>
#include <stdlib.h>

#include "json-util.h"
#include "servidor-concurrente.h"

// Constantes =========================================================
#define ARCHIVO_CONFIG "switch.json"
#define KEY_HOST "host"
#define KEY_PORT "puerto"

// Globales ===========================================================
const char* HOST;
int PORT;

// Funciones ==========================================================
void importar_config();


int main(int argc, char **argv)
{
    // Importo la ubicación del servidor switch desde un JSON:
    importar_config();
    // Arranco el servidor:
    servidor(HOST, PORT);
}

void importar_config()
{
    json_object* objeto = json_object_from_file(ARCHIVO_CONFIG);
    if (!objeto)
    {
        printf("No se pudo importar la configuración...\n");
        exit(1);
    }

    HOST = json_get_string(objeto, KEY_HOST);
    if (!HOST)
    {
        printf("No se pudo leer el host (Clave: %s)...\n", KEY_HOST);
        exit(1);
    }

    PORT = json_get_int(objeto, KEY_PORT);
    if (!PORT)
    {
        printf("No se pudo leer el nombre de la base de datos (Clave: %s)...\n", KEY_PORT);
        exit(1);
    }
}


