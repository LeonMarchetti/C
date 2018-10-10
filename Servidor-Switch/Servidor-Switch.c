// Librerías ==========================================================
#include <json.h>
#include <stdio.h>
#include <stdlib.h>

#include "json-util.h"
#include "servidor-concurrente.h"

// Constantes =========================================================
#define ARCHIVO_CONFIG "switch.json"
#define ARCHIVO_INFO "servidores.json"
#define KEY_HOST "host"
#define KEY_PORT "puerto"

// Globales ===========================================================
const char* HOST;
int PORT;
json_object* servidores_json;

// Funciones ==========================================================
void importar_config();
void importar_servidores_json();


int main(int argc, char **argv)
{
    // Importo la ubicación del servidor switch desde un JSON:
    importar_config();
    // Importo la lista de servidores desde un JSON:
    importar_servidores_json();
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

    printf("Configuración importada\n");
}

void importar_servidores_json()
{
    servidores_json = json_object_from_file(ARCHIVO_CONFIG);
    if (!servidores_json)
    {
        printf("No se pudo importar la lista de servidores...\n");
        exit(1);
    }
    printf("Lista de servidores importado\n");
}


