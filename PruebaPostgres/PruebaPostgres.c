// Librerías ==========================================================
#include <json.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json-util.h"

// Constantes =========================================================
#define ARCHIVO      "config.json"
#define KEY_HOST     "host"
#define KEY_PORT     "puerto"
#define KEY_BASE     "base"
#define KEY_USER     "usuario"
#define KEY_PASS     "contraseña"
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
PGconn* conectar();
void analizar_resultado(json_object* resultado);

void prueba_bases_de_datos(PGconn*);
void prueba_consulta(PGconn*);
void prueba_columnas(PGconn*);

json_object* postgres_bases_de_datos(void* conexion);
json_object* postgres_consulta(void* conexion, const char* consulta);
json_object* postgres_tablas(void* conexion);
json_object* postgres_columnas(void* conexion, const char* tabla);
json_object* postgres_res_a_json(PGresult* resultado);

int main(int argc, char **argv)
{
    importar_config();
    PGconn* conexion = conectar();

    // Prueba:
    prueba_bases_de_datos(conexion);
    //~ prueba_consulta(conexion);
    //~ prueba_columnas(conexion);

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
    printf("Host:          '%s'\n", host);

    puerto = json_get_string(objeto, KEY_PORT);
    if (!puerto)
    {
        printf("No se pudo leer el puerto (Clave: %s)...\n", KEY_HOST);
        exit(1);
    }
    printf("Puerto:          '%s'\n", puerto);

    base = json_get_string(objeto, KEY_BASE);
    if (!base)
    {
        printf("No se pudo leer el nombre de la base de datos (Clave: %s)...\n", KEY_BASE);
        exit(1);
    }
    printf("Base de datos: '%s'\n", base);

    usuario = json_get_string(objeto, KEY_USER);
    if (!usuario)
    {
        printf("No se pudo leer el nombre de usuario (Clave: %s)...\n", KEY_USER);
        exit(1);
    }
    printf("Usuario:       '%s'\n", usuario);

    contrasena = json_get_string(objeto, KEY_PASS);
    if (!contrasena)
    {
        printf("No se pudo leer la contraseña (Clave: %s)...\n", KEY_PASS);
        exit(1);
    }
    //~ printf("Contraseña: '%s'\n", contrasena);

    consulta = json_get_string(objeto, KEY_CONSULTA);
    if (!consulta)
    {
        printf("No se pudo leer la consulta (Clave: %s)...\n", KEY_CONSULTA);
        exit(1);
    }
    printf("Consulta:      '%s'\n", consulta);
    printf("----------------------------------------\n");
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

void analizar_resultado(json_object* resultado)
{
    if (!resultado)
    {
        printf("No se obtuvo resultado...\n");
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

    // Columnas:
    json_object* arreglo_columnas;
    json_object_object_get_ex(resultado, "columnas", &arreglo_columnas);
    int i = 0;
    json_object* obj_columna;
    printf("|Fila");
    while ((obj_columna = json_object_array_get_idx(arreglo_columnas, i++)))
    {
        printf("|%-20s", json_object_get_string(obj_columna));
    }
    printf("|\n");

    // Iterar filas:
    int          f = 0;
    json_object* arreglo_fila;
    while((arreglo_fila = json_object_array_get_idx(filas, f++)))
    {
        printf("|%-4d", f);
        int          c = 0;
        json_object* obj_celda;
        while ((obj_celda = json_object_array_get_idx(arreglo_fila, c++)))
        {
            printf("|%-20s", json_object_get_string(obj_celda));
        }
        printf("|\n");
    }

    // Como queda el string JSON del objeto, que voy a enviar por el socket:
    print_json(resultado);
}

void prueba_bases_de_datos(PGconn* conexion)
{
    json_object* resultado = postgres_bases_de_datos(conexion);
    analizar_resultado(resultado);
    free(resultado);
}

void prueba_consulta(PGconn* conexion)
{
    json_object* resultado = postgres_consulta(conexion, consulta);
    analizar_resultado(resultado);
    free(resultado);
}

void prueba_columnas(PGconn* conexion)
{
    json_object* resultado = postgres_columnas(conexion, "public.\"Factura\"");
    analizar_resultado(resultado);
    free(resultado);
}

json_object* postgres_bases_de_datos(void* conexion)
{
    const char* consulta = "Select datname "
                               "From pg_database "
                               "Where datistemplate = false";
    return postgres_consulta(conexion, consulta);
}

json_object* postgres_consulta(void* conexion, const char* consulta)
{
    PGresult* resultado = PQexec((PGconn*) conexion, consulta);
    if (resultado == NULL ||
        PQresultStatus(resultado) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "%s\n", PQerrorMessage((PGconn*) conexion));
        return NULL;
    }
    return postgres_res_a_json(resultado);
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

    //~ return postgres_res_a_json(resultado);

    // Arreglo de columnas (solo una):
    json_object* arreglo_columnas = json_object_new_array();
    json_object_array_add(arreglo_columnas, json_object_new_string("tablename"));

    // Arreglo con cada fila del resultado:
    json_object* arreglo    = json_object_new_array(); // Arreglo con las filas.
    int          cant_filas = PQntuples(resultado);
    for (int f = 0; f < cant_filas; f++)
    {
        // Arreglo JSON con cada valor de la fila:
        json_object* arreglo_fila = json_object_new_array();

        char nombre_tabla[64];
        // Armar nombre de la tabla:
        sprintf(nombre_tabla, "%s.\"%s\"",
                PQgetvalue(resultado, f, 0),
                PQgetvalue(resultado, f, 1));

        json_object_array_add(
            arreglo_fila,
            json_object_new_string(nombre_tabla)
        );
        json_object_array_add(arreglo, arreglo_fila);
    }
    PQclear(resultado);

    // Regresar resultado como objeto JSON:
    json_object* objeto            = json_object_new_object();
    json_object* objeto_cant_filas = json_object_new_int(cant_filas);

    json_object_object_add(objeto, "cantidad", objeto_cant_filas);
    json_object_object_add(objeto, "columnas", arreglo_columnas);
    json_object_object_add(objeto, "filas", arreglo);
    return objeto;
}

json_object* postgres_columnas(void* conexion, const char* tabla)
{
    /* Para realizar una consulta en postgres, tengo que referirme a las tablas
     * como: esquema."tabla", por ej.: public."Cliente", por lo tanto tengo que
     * separar el esquema y la tabla para hacer las consultas de metadatos.
     */

    // String temporal para strtok:
    char tmp_tabla[strlen(tabla)];
    strcpy(tmp_tabla, tabla);

    char* schema = malloc(strlen(tabla)); // Esquema de la tabla
    char* table  = malloc(strlen(tabla)); // Nombre de la tabla

    // Separo el esquema y la tabla según el punto:
    int   i      = 0;
    char* token  = strtok(tmp_tabla, ".");
    while (token != NULL)
    {
        switch (i++)
        {
            case 0: strcpy(schema, token); break;
            case 1: strcpy(table, token);
        }
        token = strtok(NULL, ".");
    }

    // Reemplazo las comillas dobles por simples:
    table[0] = '\'';
    table[strlen(table)-1] = '\'';

    // Hago la consulta:
    const char* plantilla_sql = "Select * "
                                    "From information_schema.columns "
                                    "Where table_schema = '%s' And table_name = %s";
    char*       consulta_2    = malloc(strlen(schema) + strlen(table) + strlen(plantilla_sql));
    sprintf(consulta_2, plantilla_sql, schema, table);

    json_object* resultado = postgres_consulta(conexion, consulta_2);

    free(schema);
    free(table);
    free(consulta_2);

    return resultado;

    //~ // Mensaje de error:
    //~ fprintf(stderr, "postgres_columnas(void*, const char*): Esta función no está disponible...\n");
    //~ return NULL;

    //~ // Funciona:
    //~ json_object* resultado = postgres_consulta(conexion, tabla);
    //~ return resultado;
}

json_object* postgres_res_a_json(PGresult* resultado)
{
    // Longitud de string para cada fila:
    int          cant_columnas    = PQnfields(resultado);
    json_object* arreglo_columnas = json_object_new_array();
    for (int c = 0; c < cant_columnas; c++)
    {
        json_object_array_add(arreglo_columnas,
                              json_object_new_string(PQfname(resultado, c)));
    }

    // Arreglo con cada fila del resultado:
    json_object* arreglo    = json_object_new_array(); // Arreglo con las filas.
    int          cant_filas = PQntuples(resultado);
    for (int f = 0; f < cant_filas; f++)
    {
        // Arreglo con cada valor de la fila:
        json_object* arreglo_fila = json_object_new_array();
        for (int c = 0; c < cant_columnas; c++)
        {
            json_object_array_add(
                arreglo_fila,
                json_object_new_string(
                    PQgetvalue(resultado, f, c)
                )
            );
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
