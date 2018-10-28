// Librerías ==========================================================
#include <arpa/inet.h>
#include <json.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "json-util.h"
#include "modulo-mysql.h"
#include "modulo-postgres.h"
#include "servidor-concurrente.h"

// Constantes =========================================================
#define ARCHIVO_CONFIG "switch.json"
#define ARCHIVO_INFO   "servidores.json"
#define KEY_HOST       "host"
#define KEY_PORT       "puerto"

// Tipos de SGBD:
#define TIPO_MYSQL    'm'
#define TIPO_POSTGRES 'p'
#define TIPO_FIREBIRD 'f'

// Tipos ==============================================================
struct basedatos_t
{
    char  tipo;
    char  nom_servidor[50];
    char  nom_bd[50];
    void* conexion;
};

// Globales ===========================================================
const char*         HOST;
int                 PORT;
int                 cantidad_bds;
json_object*        obj_servidores;
struct basedatos_t* bases_de_datos;

// Funciones ==========================================================
void  terminar();
void  importar_config();
void  importar_servidores_json();
void* atender(void*);
void  print_servidores_json();
void  conectar_basesdedatos();
void  cantidad_basesdedatos();
void  print_arreglo_bds();


int main(int argc, char **argv)
{
    // Importo la ubicación del servidor switch desde un JSON:
    importar_config();
    // Importo la lista de servidores desde un JSON:
    importar_servidores_json();
    //~ print_servidores_json();
    cantidad_basesdedatos();
    conectar_basesdedatos();
    print_arreglo_bds();

    //~ servidor(HOST, PORT, atender);

    terminar(0);
}

void terminar(int exit_code)
{
    json_object_put(obj_servidores);
    free(bases_de_datos);
    exit(exit_code);
}

void importar_config()
{
    json_object* objeto = json_object_from_file(ARCHIVO_CONFIG);
    if (!objeto)
    {
        printf("No se pudo importar la configuración...\n");
        terminar(1);
    }

    HOST = json_get_string(objeto, KEY_HOST);
    if (!HOST)
    {
        printf("No se pudo leer el host (Clave: %s)...\n", KEY_HOST);
        terminar(1);
    }

    PORT = json_get_int(objeto, KEY_PORT);
    if (!PORT)
    {
        printf("No se pudo leer el puerto (Clave: %s)...\n", KEY_PORT);
        terminar(1);
    }

    printf("Configuración importada\n");

    json_object_put(objeto);
}

void importar_servidores_json()
{
    obj_servidores = json_object_from_file(ARCHIVO_INFO);
    if (!obj_servidores)
    {
        printf("No se pudo importar la lista de servidores...\n");
        terminar(1);
    }
    printf("Lista de servidores importada\n");
}

void* atender(void *arg)
{
    // Para que el hilo termine cuando termine la función:
    pthread_detach(pthread_self());

    char buf_in[BUFFER];
    char buf_out[BUFFER];
    int nb;

    // Recibo los datos pasados al hilo por el puntero:
    struct datos_t *datos = (struct datos_t*) arg;

    printf("[Hilo][%d] Atendiendo socket\n", datos->socket);

    while (1)
    {
        // Recibir datos:
        nb = read(datos->socket, buf_in, BUFFER);
        buf_in[nb] = '\0';

        // Presionar solo enter para terminar la sesión
        if (!strcmp(buf_in, CERRAR))
        {
            break;
        }
        buf_in[nb-1] = '\0'; // Saco el salto de línea

        // Convierto los datos recibidos a JSON:
        json_object* objeto = json_tokener_parse(buf_in);
        if (objeto)
        {
            // Mostrar en pantalla:
            printf("[Hilo][%d] %s\n",
                datos->socket,
                json_object_to_json_string_ext(objeto, 0));

            // comando:
            const char* comando = json_get_string(objeto, "comando");
            if (comando)
            {
                switch (comando[0])
                {
                    case 'a': // Listar atributos de tabla
                        break;
                    case 'b': // Listar bases de datos
                        break;
                    case 'q': // Consulta
                        break;
                    case 't': // Listar tablas de base de datos
                        break;
                }
            }
            else
            {
                printf("Datos inválidos: '%s'\n", buf_in);
                // Se recibió un objeto inválido
                sprintf(buf_out, "Dato inválido");
            }

            // Enviar datos:
            write(datos->socket, buf_out, BUFFER);
        }
        else
        {
            printf("Datos inválidos: '%s'\n", buf_in);
            // Se recibió un objeto inválido
            sprintf(buf_out, "Dato inválido");
        }
    }

    // Cerrar socket:
    close(datos->socket);
    printf("[Hilo][%d] Socket cerrado\n", datos->socket);

    free(datos);
    return NULL;
}

void print_servidores_json()
{
    json_object* arr_servidores;
    json_object_object_get_ex(obj_servidores, "servidores", &arr_servidores);
    if (!arr_servidores)
    {
        fprintf(stderr, "No se encontró el objeto 'servidores'...\n");
        terminar(1);
    }

    int          i = 0;
    json_object* obj_servidor;
    while ((obj_servidor = json_object_array_get_idx(arr_servidores, i++)))
    {
        printf("--------------------------------------------------------------------------------");
        printf("Nombre\t%s\n", json_get_string(obj_servidor, "nombre"));
        printf("Tipo\t%s\n",   json_get_string(obj_servidor, "tipo"));
        printf("Host\t%s\n",   json_get_string(obj_servidor, "host"));
        printf("Puerto\t%d\n", json_get_int   (obj_servidor, "puerto"));

        int          j = 0;
        json_object* obj_bd;
        json_object* arr_bases;
        json_object_object_get_ex(obj_servidor, "bases_de_datos", &arr_bases);

        while ((obj_bd = json_object_array_get_idx(arr_bases, j++)))
        {
            json_object_object_foreach(obj_bd, clave, valor)
            {
                printf("\t%s: \"%s\"", clave, json_object_get_string(valor));
            }
            printf("\n");
        }
    }
    printf("--------------------------------------------------------------------------------");
}

void cantidad_basesdedatos()
{
    cantidad_bds = 0;

    json_object* arr_servidores;
    json_object_object_get_ex(obj_servidores, "servidores", &arr_servidores);
    if (!arr_servidores)
    {
        fprintf(stderr, "No se encontró el objeto 'servidores'...\n");
        terminar(1);
    }

    int          i = 0;
    json_object* obj_servidor;
    while ((obj_servidor = json_object_array_get_idx(arr_servidores, i++)))
    {
        json_object* arr_bases;
        json_object_object_get_ex(obj_servidor, "bases_de_datos", &arr_bases);

        if (arr_bases)
        {
            cantidad_bds += json_object_array_length(arr_bases);
        }
    }

    // Cierro el servidor si no hay bases de datos:
    if (cantidad_bds == 0)
    {
        printf("No hay bases de datos disponibles");
        terminar(1);
    }
}

void conectar_basesdedatos()
{
    json_object* arr_servidores;
    json_object_object_get_ex(obj_servidores, "servidores", &arr_servidores);

    bases_de_datos = malloc(sizeof(struct basedatos_t) * cantidad_bds);
    int b = 0;

    // Iterar arreglo de servidores en JSON:
    int          i = 0;
    json_object* obj_servidor;
    while ((obj_servidor = json_object_array_get_idx(arr_servidores, i++)))
    {
        const char* host = json_get_string(obj_servidor, "host");
        int         port = json_get_int(obj_servidor, "puerto");

        // Iterar arreglo de bases de datos del servidor en JSON:
        int          j = 0;
        json_object* obj_bd;
        json_object* arr_bases;
        json_object_object_get_ex(obj_servidor, "bases_de_datos", &arr_bases);

        while ((obj_bd = json_object_array_get_idx(arr_bases, j++)))
        {
            bases_de_datos[b].tipo = json_get_string(obj_servidor, "tipo")[0];
            strcpy(bases_de_datos[b].nom_servidor, json_get_string(obj_servidor, "nombre"));

            strcpy(bases_de_datos[b].nom_bd, json_get_string(obj_bd, "base"));
            const char* usuario     = json_get_string(obj_bd, "usuario");
            const char* contrasenia = json_get_string(obj_bd, "contraseña");

            // Realizar conexión a base de datos según tipo:
            switch (bases_de_datos[b].tipo)
            {
                case TIPO_MYSQL:
                    bases_de_datos[b].conexion = mysql_conectar(host, port, usuario, contrasenia, bases_de_datos[b].nom_bd);
                    break;
                case TIPO_POSTGRES:
                    bases_de_datos[b].conexion = postgres_conectar(host, port, usuario, contrasenia, bases_de_datos[b].nom_bd);
                    break;
                case TIPO_FIREBIRD:
                    //~ bases_de_datos[b].conexion = firebird_conectar(host, port, usuario, contrasenia, bases_de_datos[b].nom_bd);
                    bases_de_datos[b].conexion = NULL;
                    break;
                default:
                    bases_de_datos[b].conexion = NULL;
                    printf("Tipo desconocido: '%c'\n", bases_de_datos[b].tipo);
            }
            b++; // Índice del arreglo global de bases de datos.
        }
    }
}

void print_arreglo_bds()
{
    for (int i = 0; i < cantidad_bds; i++)
    {
        printf("%c\t%15s\t%15s\t%p\n", bases_de_datos[i].tipo, bases_de_datos[i].nom_servidor, bases_de_datos[i].nom_bd, bases_de_datos[i].conexion);
    }
}
