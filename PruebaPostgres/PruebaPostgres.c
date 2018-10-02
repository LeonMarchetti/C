// Librerías ==========================================================
#include <json.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>

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
const char* json_get_string(json_object*, char*);
PGconn* conectar();
void consultar(PGconn*, const char*);

int main(int argc, char **argv)
{
    importar_config();
    importar_consulta();
    PGconn* conexion = conectar();
    consultar(conexion, consulta);
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
}

const char* json_get_string(json_object* objeto, char* clave)
{
    json_object* objeto_hijo;
    json_object_object_get_ex(objeto, clave, &objeto_hijo);
    return json_object_get_string(objeto_hijo);
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

void consultar(PGconn* conexion, const char* consulta)
{
    PGresult* resultado = PQexec(conexion, consulta);
    if (resultado == NULL ||
        PQresultStatus(resultado) != PGRES_TUPLES_OK)
    {
        printf("Falló la consulta: %s\n", PQerrorMessage(conexion));
        PQfinish(conexion);
        exit(1);
    }

    int filas = PQntuples(resultado);
    int columnas = PQnfields(resultado);
    printf("Cantidad de filas:    %d\n", filas);
    printf("Cantidad de columnas: %d\n", columnas);

    // Columnas:
    for (int c = 0; c < columnas; c++)
    {
        printf("%s\t", PQfname(resultado, c));
    }
    printf("\n");

    for (int f = 0; f < filas; f++)
    {
        for (int c = 0; c < columnas; c++)
        {
            printf("%s\t", PQgetvalue(resultado, f, c));
        }
        printf("\n");
    }
    PQclear(resultado);
}
