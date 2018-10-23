// Librerías ==========================================================
#include <json.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json-util.h"

// Constantes =========================================================
#define ARCHIVO "config.json"
#define KEY_HOST "host"
#define KEY_PORT "puerto"
#define KEY_BASE "base"
#define KEY_USER "usuario"
#define KEY_PASS "contraseña"
#define KEY_CONSULTA "sql"

// Globales ===========================================================
// Servidor Postgres:
const char* host;
const char* puerto;
const char* base;
const char* usuario;
const char* contrasena;
const char* consulta;

// Funciones ==========================================================
void importar_config();
void importar_consulta();
PGconn* conectar();
void prueba_consulta(PGconn*);

json_object* postgres_consulta(void* conexion, const char* consulta);
json_object* postgres_tablas(void* conexion);

int main(int argc, char **argv)
{
    importar_config();
    importar_consulta();
    PGconn* conexion = conectar();
    prueba_consulta(conexion);
    PQfinish(conexion);
}

void importar_config()
{
    json_object* objeto = json_object_from_file(ARCHIVO);
    if (!objeto)
    {
        printf("No se pudo importar la configuración...\n");
        exit(1);
    }

    host = json_get_string(objeto, KEY_HOST);
    if (!host)
    {
        printf("No se pudo leer el host (Clave: %s)...\n", KEY_HOST);
        exit(1);
    }

    puerto = json_get_string(objeto, KEY_PORT);
    if (!host)
    {
        printf("No se pudo leer el puerto (Clave: %s)...\n", KEY_PORT);
        exit(1);
    }

    base = json_get_string(objeto, KEY_BASE);
    if (!base)
    {
        printf("No se pudo leer el nombre de la base de datos (Clave: %s)...\n", KEY_BASE);
        exit(1);
    }

    usuario = json_get_string(objeto, KEY_USER);
    if (!usuario)
    {
        printf("No se pudo leer el nombre de usuario (Clave: %s)...\n", KEY_USER);
        exit(1);
    }

    contrasena = json_get_string(objeto, KEY_PASS);
    if (!contrasena)
    {
        printf("No se pudo leer la contraseña (Clave: %s)...\n", KEY_PASS);
        exit(1);
    }
}

void importar_consulta()
{
    json_object* objeto = json_object_from_file("consulta.json");
    if (!objeto)
    {
        printf("No se pudo importar la consulta...\n");
        exit(1);
    }

    consulta = json_get_string(objeto, KEY_CONSULTA);
    if (!consulta)
    {
        printf("No se pudo leer la consulta (Clave: %s)...\n", KEY_CONSULTA);
        exit(1);
    }
    printf("Consulta importada: \n\"%s\"\n\n", consulta);
}

PGconn* conectar()
{
    PGconn* conexion = PQsetdbLogin(host, puerto, NULL, NULL, base,
                                    usuario, contrasena);
    if (PQstatus(conexion) == CONNECTION_BAD)
    {
        printf("Falló la conexión: %s\n", PQerrorMessage(conexion));
        PQfinish(conexion);
        exit(1);
    }

    return conexion;
}

void prueba_consulta(PGconn* conexion)
{
    json_object* resultado = postgres_consulta(conexion, consulta);
    //~ json_object* resultado = postgres_tablas(conexion);
    if (!resultado)
    {
        return;
    }

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

    printf("\nprueba_consulta:\n");
    for (int i = 0; i < cantidad; i++)
    {
        json_object* arreglo_fila = json_object_array_get_idx(filas, i);

        // Iterar arreglo fila:
        int len = json_object_array_length(arreglo_fila);
        for (int j = 0; j < len; j++)
        {
            json_object* objeto_celda = json_object_array_get_idx(arreglo_fila, j);
            const char* contenido = json_object_get_string(objeto_celda);
            printf("%s\t", contenido);
        }
        printf("\n");
    }

    print_json(resultado);
    free(resultado);
}

json_object* postgres_consulta(void* conexion, const char* consulta)
{
    PGresult* resultado = PQexec((PGconn* ) conexion, consulta);
    if (resultado == NULL ||
        PQresultStatus(resultado) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "%s\n", PQerrorMessage((PGconn* ) conexion));
        return NULL;
    }

    int cant_columnas = PQnfields(resultado);
    json_object* arreglo_columnas = json_object_new_array();
    for (int c = 0; c < cant_columnas; c++)
    {
        json_object_array_add(arreglo_columnas,
                              json_object_new_string(PQfname(resultado, c)));
    }

    // Arreglo JSON con cada fila del resultado:
    json_object* arreglo = json_object_new_array(); // Arreglo con las filas.
    int cant_filas = PQntuples(resultado);
    for (int f = 0; f < cant_filas; f++)
    {
        // Arreglo JSON con cada valor de la fila:
        json_object* arreglo_fila = json_object_new_array();
        for (int c = 0; c < cant_columnas; c++)
        {
            const char* valor = PQgetvalue(resultado, f, c);
            json_object* objeto_celda = json_object_new_string(valor);
            json_object_array_add(arreglo_fila, objeto_celda);
        }
        json_object_array_add(arreglo, arreglo_fila);
    }
    PQclear(resultado);

    // Regresar resultado como objeto JSON:
    json_object* objeto = json_object_new_object();
    json_object* objeto_cant_filas = json_object_new_int(cant_filas);
    json_object_object_add(objeto, "cantidad", objeto_cant_filas);
    json_object_object_add(objeto, "columnas", arreglo_columnas);
    json_object_object_add(objeto, "filas", arreglo);
    return objeto;
}

json_object* postgres_tablas(void* conexion)
{
    const char* select_tablas = "Select schemaname, tablename From pg_catalog.pg_tables Where schemaname != 'pg_catalog' And schemaname != 'information_schema';";

    PGresult* resultado = PQexec((PGconn* ) conexion, select_tablas);
    if (resultado == NULL ||
        PQresultStatus(resultado) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "%s\n", PQerrorMessage((PGconn* ) conexion));
        return NULL;
    }

    // Arreglo de columnas (solo una):
    json_object* arreglo_columnas = json_object_new_array();
    json_object_array_add(arreglo_columnas, json_object_new_string("tabla"));

    // Arreglo con cada fila del resultado:
    json_object* arreglo = json_object_new_array(); // Arreglo con las filas.
    int cant_filas = PQntuples(resultado);
    for (int f = 0; f < cant_filas; f++)
    {
        // Arreglo JSON con cada valor de la fila:
        json_object* arreglo_fila = json_object_new_array();

        // Armar nombre de la tabla:
        char nombre_tabla[64];
        sprintf(nombre_tabla, "%s.\"%s\"",
                PQgetvalue(resultado, f, 0),
                PQgetvalue(resultado, f, 1));

        json_object_array_add(arreglo_fila, json_object_new_string(nombre_tabla));
        json_object_array_add(arreglo, arreglo_fila);
    }
    PQclear(resultado);

    // Regresar resultado como objeto JSON:
    json_object* objeto = json_object_new_object();
    json_object* objeto_cant_filas = json_object_new_int(cant_filas);
    json_object_object_add(objeto, "cantidad", objeto_cant_filas);
    json_object_object_add(objeto, "columnas", arreglo_columnas);
    json_object_object_add(objeto, "filas", arreglo);
    return objeto;
}
