// Librerías ==========================================================
#include <json.h>
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>

// Constantes =========================================================
#define ARCHIVO "config.json"
#define KEY_HOST "host"
#define KEY_BASE "base"
#define KEY_USER "usuario"
#define KEY_PASS "contraseña"

// Globales ===========================================================
// Servidor MySQL:
const char* host;
const char* base;
const char* usuario;
const char* contrasena;

// Funciones ==========================================================
MYSQL* conectar();
void consultar(MYSQL*, char*);
void importar_config();
const char* json_get_string(json_object*, char*);

int main(int argc, char **argv)
{
    importar_config();
    MYSQL* conexion = conectar();
    consultar(conexion, "Select * From Factura");
    mysql_close(conexion);
}

MYSQL* conectar()
{
    MYSQL* conexion = mysql_init(NULL);
    if (!mysql_real_connect(conexion, host, usuario, contrasena, base, 0, NULL, 0))
    {
        //~ fprintf(stderr, "%s\n", mysql_error(conexion));
        printf("%s\n", mysql_error(conexion));
        return NULL;
    }
    return conexion;
}

void consultar(MYSQL* conexion, char* consulta)
{
    // Ejecutar consulta:
    if (mysql_query(conexion, consulta))
    {
        //~ fprintf(stderr, "%s\n", mysql_error(conexion));
        printf("Error en la consulta:\n%s\n", mysql_error(conexion));
        exit(1);
    }

    // Obtener resultado de la consulta:
    //~ MYSQL_RES* resultado = mysql_use_result(conexion);
    MYSQL_RES* resultado = mysql_store_result(conexion);

    // Información del resultado:
    int columnas = mysql_num_fields(resultado);
    printf("Cantidad de columnas:\t%d\n", columnas);
    printf("Cantidad de filas:\t%lld\n", mysql_num_rows(resultado));
    printf("=========================\n");

    // Obtener nombre de las columnas:
    MYSQL_FIELD* campo;
    while ((campo = mysql_fetch_field(resultado)))
    {
        printf("%s\t", campo->name);
    }

    printf("\n");

    // Obtener filas:
    MYSQL_ROW fila;
    while ((fila = mysql_fetch_row(resultado)) != NULL)
    {
        for (int i = 0; i < columnas; i++)
        {
            printf("%s\t", fila[i]);
        }
        printf("\n");
    }

    // Liberar resultado:
    mysql_free_result(resultado);
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

const char* json_get_string(json_object* objeto, char* clave)
{
    json_object* objeto_hijo;
    json_bool bool_resultado = json_object_object_get_ex(objeto, clave, &objeto_hijo);
    if (!bool_resultado || !objeto_hijo)
    {
        return NULL;
    }
    return json_object_get_string(objeto_hijo);
}
