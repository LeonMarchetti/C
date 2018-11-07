// Librerías ==========================================================
#include <json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json-util.h"
#include "servidor-concurrente.h"

// Constantes =========================================================
#define ARCHIVO_CONFIG "switch.json"
#define KEY_HOST       "host"
#define KEY_PORT       "puerto"

// Globales ===========================================================
const char* HOST;
int PORT;

// Funciones ==========================================================
void importar_config();
void cliente_switch();
void analizar_resultado(json_object* resultado);

void pedir_dbs(char* mensaje);
void recibir_dbs(char* mensaje);

void enviar(char* mensaje);
void recibir(char* mensaje);


int main(int argc, char **argv)
{
    // Importo la ubicación del servidor switch desde un JSON:
    importar_config();

    // Arranco el cliente para pedir la lista de servidores:
    cliente_uniq(HOST, PORT, pedir_dbs, recibir_dbs);

    // Arranco el cliente para empezar el programa:
    cliente(HOST, PORT, enviar, recibir);
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

void pedir_dbs(char* mensaje)
{
    json_object* obj_out = json_object_new_object();
    json_object_object_add(obj_out, "comando", json_object_new_string("s"));
    sprintf(mensaje, json_object_to_json_string_ext(obj_out, 0));
}

void recibir_dbs(char* mensaje)
{
    // Obtengo el objeto JSON de los datos recibidos:
    json_object* obj_in = json_tokener_parse(mensaje);
    if (!obj_in)
    {
        fprintf(stderr, "No se recibió un objeto JSON válido...\n'%s'\n", mensaje);
        exit(1);
    }

    json_object* obj_error;
    json_object_object_get_ex(obj_in, "error", &obj_error);
    if (obj_error)
    {
        fprintf(stderr, "%s\n", json_get_string(obj_in, "error"));
        exit(1);
    }

    json_object* obj_resultado;
    json_object_object_get_ex(obj_in, "resultado", &obj_resultado);

    if (!json_get_int(obj_resultado, "cantidad"))
    {
        fprintf(stderr, "No se encontraron servidores...\n");
        exit(1);
    }

    int          i = 0;
    json_object* arr_servidores;
    json_object* obj_servidor;

    printf("Servidores:\n");
    json_object_object_get_ex(obj_resultado, "filas", &arr_servidores);
    while ((obj_servidor = json_object_array_get_idx(arr_servidores, i++)))
    {
        printf("> %s\n", json_object_get_string(obj_servidor));
    }
}

void enviar(char* mensaje)
{
    printf("Ingresar comando (a,b,q,s,t) > ");

    char* in_comando = malloc(10 * sizeof(char));
    fgets(in_comando, 10, stdin);

    char*        nom_bd       = malloc(20 * sizeof(char));
    char*        nom_servidor = malloc(20 * sizeof(char));
    char*        nom_tabla    = malloc(20 * sizeof(char));
    char*        query        = malloc(256 * sizeof(char));
    json_object* obj_out      = json_object_new_object();

    //~ printf("Servidor> ");
    //~ fgets(nom_servidor, 20, stdin);
    //~ json_object_object_add(obj_out, "servidor", json_object_new_string(nom_servidor));

    switch (in_comando[0])
    {
        case 'a': // Lista de atributos
            printf("Servidor> ");
            fgets(nom_servidor, 20, stdin);

            printf("Base de datos> ");
            fgets(nom_bd, 20, stdin);

            printf("Tabla> ");
            fgets(nom_tabla, 20, stdin);

            json_object_object_add(obj_out, "comando",       json_object_new_string("a"));
            json_object_object_add(obj_out, "servidor",      json_object_new_string(nom_servidor));
            json_object_object_add(obj_out, "base_de_datos", json_object_new_string(nom_bd));
            json_object_object_add(obj_out, "tabla",         json_object_new_string(nom_tabla));
            break;

        case 'b': // Lista de bases de datos
            printf("Servidor> ");
            fgets(nom_servidor, 20, stdin);
            nom_servidor[18] = 0;
            printf("Servidor: '%s'\n", nom_servidor);

            json_object_object_add(obj_out, "comando",       json_object_new_string("b"));
            json_object_object_add(obj_out, "servidor",      json_object_new_string(nom_servidor));
            break;

        case 'q': // Consulta
            printf("Servidor> ");
            fgets(nom_servidor, 20, stdin);

            printf("Base de datos> ");
            fgets(nom_bd, 20, stdin);

            printf("Consulta> ");
            fgets(query, 256, stdin);

            json_object_object_add(obj_out, "comando",       json_object_new_string("q"));
            json_object_object_add(obj_out, "servidor",      json_object_new_string(nom_servidor));
            json_object_object_add(obj_out, "base_de_datos", json_object_new_string(nom_bd));
            json_object_object_add(obj_out, "consulta     ", json_object_new_string(query));
            break;

        case 't': // Lista de tablas
            printf("Servidor> ");
            fgets(nom_servidor, 20, stdin);

            printf("Base de datos> ");
            fgets(nom_bd, 20, stdin);

            json_object_object_add(obj_out, "comando",       json_object_new_string("t"));
            json_object_object_add(obj_out, "servidor",      json_object_new_string(nom_servidor));
            json_object_object_add(obj_out, "base_de_datos", json_object_new_string(nom_bd));
            break;

        case 's': // Lista de servidores
        default:
            json_object_object_add(obj_out, "comando",       json_object_new_string("s"));
            json_object_object_add(obj_out, "servidor",      json_object_new_string(nom_servidor));
            break;
    }

    sprintf(mensaje, json_object_to_json_string_ext(obj_out, 0));
    mensaje[strlen(mensaje)] = 0;

    free(in_comando);
    free(nom_bd);
    free(nom_servidor);
    free(nom_tabla);
    free(query);
}

void recibir(char* mensaje)
{
    json_object* obj_in = json_tokener_parse(mensaje);
    if (obj_in)
    {
        json_object* obj_error;
        json_object_object_get_ex(obj_in, "error", &obj_error);
        if (obj_error)
        {
            fprintf(stderr, "Error: %s\n", json_object_get_string(obj_error));
        }
        else
        {
            json_object* obj_resultado;
            json_object_object_get_ex(obj_in, "resultado", &obj_resultado);
            analizar_resultado(obj_resultado);
        }
        free(obj_in);
    }
    else
    {
        // El mensaje no es un string JSON válido:
        fprintf(stderr, "Datos inválidos:\n%s\n", mensaje);
    }
}

void analizar_resultado(json_object* resultado)
{
    if (!resultado)
    {
        printf("No se obtuvo resultado...\n");
        return;
    }

    printf("Objeto recibido:\n'%s'\n\n", json_object_to_json_string(resultado));

    json_object* objeto_cantidad;
    json_object_object_get_ex(resultado, "cantidad", &objeto_cantidad);
    int cantidad = json_object_get_int(objeto_cantidad);
    if (cantidad <= 0)
    {
        printf("Resultado vacío\n");
        return;
    }

    json_object* filas;
    json_object_object_get_ex(resultado, "filas", &filas);

    // Columnas:
    json_object* arreglo_columnas;
    json_object_object_get_ex(resultado, "columnas", &arreglo_columnas);
    int i = 0;
    json_object* obj_columna;
    printf("|Fila");
    while ((obj_columna = json_object_array_get_idx(arreglo_columnas, i++)))
    {
        printf("|%-10s", json_object_get_string(obj_columna));
    }
    printf("|\n");

    // Iterar filas:
    int          f = 0;
    json_object* arr_fila;
    while((arr_fila = json_object_array_get_idx(filas, f++)))
    {
        printf("|%-4d", f);
        //~ int          c = 0;

        for (int c = 0; c < json_object_array_length(arr_fila); c++)
        {
            json_object* obj_celda;
            obj_celda = json_object_array_get_idx(arr_fila, c);
            printf("|'%-10s'", json_object_get_string(obj_celda));
        }

        /*json_object* obj_celda;
        // vvv segfault vvv
        while ((obj_celda = json_object_array_get_idx(arr_fila, c++)))
        {
            printf("|'%-10s'", json_object_get_string(obj_celda));
        }*/
        printf("|\n");
    }
}
