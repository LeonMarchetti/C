// Librerías ==========================================================
#include "modulo-postgres.h"

#include <json.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <string.h>

// Constantes =========================================================

// Funciones ==========================================================
json_object* postgres_res_a_json(PGresult* resultado);


void* postgres_conectar(const char* host, int puerto, const char* usuario,
                        const char* contrasenia, const char* nombre_bd)
{
    char pg_puerto[5];
    sprintf(pg_puerto, "%d", puerto);
    PGconn* conexion = PQsetdbLogin(host, pg_puerto, NULL, NULL, nombre_bd, usuario,
                                    contrasenia);
    if (PQstatus(conexion) == CONNECTION_BAD)
    {
        fprintf(stderr, "%s\n", PQerrorMessage(conexion));
        PQfinish(conexion);
        return NULL;
    }
    return conexion;
}

void postgres_cerrar(void* conexion)
{
    PQfinish(conexion);
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

    // Arreglo de columnas (solo una):
    json_object* arreglo_columnas = json_object_new_array();
    json_object_array_add(arreglo_columnas, json_object_new_string("tabla"));

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
}

json_object* postgres_bases_de_datos(void* conexion)
{
    const char* consulta = "Select datname "
                               "From pg_database "
                               "Where datistemplate = false";
    return postgres_consulta(conexion, consulta);
}

json_object* postgres_res_a_json(PGresult* resultado)
{
    // Longitud de string para cada fila:
    int          cant_columnas    = PQnfields(resultado);
    json_object* arreglo_columnas = json_object_new_array();
    for (int c = 0; c < cant_columnas; c++)
    {
        json_object_array_add(
            arreglo_columnas,
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

