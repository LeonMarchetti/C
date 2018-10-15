// Librerías ==========================================================
#include <json.h>
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json-util.h"

// Constantes =========================================================
#define ARCHIVO "config.json"
#define KEY_HOST "host"
#define KEY_BASE "base"
#define KEY_USER "usuario"
#define KEY_PASS "contraseña"
const char* CONSULTA = "Select * From Cliente";

// Globales ===========================================================
// Servidor MySQL:
const char* host;
const char* base;
const char* usuario;
const char* contrasena;

// Funciones ==========================================================
MYSQL* conectar();
void consultar(MYSQL*, const char*);
void importar_config();
void listar_tablas(MYSQL*);

void prueba_lista_tablas(MYSQL*);
void prueba_consulta(MYSQL*);

json_object* mysql_tablas(void*);
json_object* mysql_consulta(void* conexion, const char* consulta);

int main(int argc, char **argv)
{
    importar_config();
    MYSQL* conexion = conectar();
    if (!conexion)
    {
        return 1;
    }

    //~ consultar(conexion, CONSULTA);
    //~ listar_tablas(conexion);
    //~ prueba_lista_tablas(conexion);
    prueba_consulta(conexion);
    mysql_close(conexion);
}

MYSQL* conectar()
{
    MYSQL* conexion = mysql_init(NULL);
    if (!mysql_real_connect(conexion, host, usuario, contrasena, base, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conexion));
        //~ printf("%s\n", mysql_error(conexion));
        return NULL;
    }
    return conexion;
}

void consultar(MYSQL* conexion, const char* consulta)
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
    printf("-------------------------\n");

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

void prueba_lista_tablas(MYSQL* conexion)
{
    json_object* tablas = mysql_tablas(conexion);
    if (!tablas)
    {
        fprintf(stderr, "No se obtuvieron resultados\n");
        return;
    }

    json_object* objeto_cantidad;
    json_object_object_get_ex(tablas, "cantidad", &objeto_cantidad);
    int cantidad = json_object_get_int(objeto_cantidad);
    if (cantidad <= 0)
    {
        fprintf(stderr, "No hay tablas\n");
        return;
    }

    json_object* filas;
    json_object_object_get_ex(tablas, "filas", &filas);

    for (int i = 0; i < cantidad; i++)
    {
        json_object* objeto_fila = json_object_array_get_idx(filas, i);
        const char* fila = json_object_get_string(objeto_fila);
        printf("> %s\n", fila);
    }

    print_json(tablas);
    free(tablas);
}

void prueba_consulta(MYSQL* conexion)
{
    json_object* resultado = mysql_consulta(conexion, CONSULTA);
    if (!resultado)
    {
        fprintf(stderr, "No se obtuvieron resultados\n");
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

    printf("\nPrueba:\n");
    for (int i = 0; i < cantidad; i++)
    {
        json_object* objeto_fila = json_object_array_get_idx(filas, i);
        const char* fila = json_object_get_string(objeto_fila);
        printf("%s\n", fila);
    }

    print_json(resultado);
    free(resultado);
}

json_object* mysql_tablas(void* conexion)
{
    MYSQL_RES* resultado = mysql_list_tables((MYSQL*) conexion, NULL);
    if (!resultado)
    {
        fprintf(stderr, "%s\n", mysql_error((MYSQL*) conexion));
        return NULL;
    }

    int cant_tablas = mysql_num_rows(resultado); // Cantidad de tablas.

    json_object* arreglo = json_object_new_array(); // Arreglo con las tablas.
    MYSQL_ROW fila;
    while ((fila = mysql_fetch_row(resultado)) != NULL)
    {
        json_object* objeto_nombre_tabla = json_object_new_string(fila[0]);
        json_object_array_add(arreglo, objeto_nombre_tabla);
    }

    mysql_free_result(resultado); // Liberar resultado.

    // Regresar resultado como objeto JSON:
    json_object* objeto = json_object_new_object();
    json_object* objeto_cant_tablas = json_object_new_int(cant_tablas);
    json_object_object_add(objeto, "cantidad", objeto_cant_tablas);
    json_object_object_add(objeto, "filas", arreglo);
    return objeto;
}

json_object* mysql_consulta(void* conexion, const char* consulta)
{
    // Ejecutar consulta:
    if (mysql_query((MYSQL*) conexion, consulta))
    {
        fprintf(stderr, "%s\n", mysql_error((MYSQL*) conexion));
        return NULL;
    }

    // Obtener resultado de la consulta:
    MYSQL_RES* resultado = mysql_use_result((MYSQL*) conexion);
    if (!resultado)
    {
        fprintf(stderr, "%s\n", mysql_error((MYSQL*) conexion));
        return NULL;
    }

    // Longitud de string para cada fila:
    int cant_columnas = mysql_num_fields(resultado);
    int tam_fila      = 0;
    MYSQL_FIELD* columna;
    while ((columna = mysql_fetch_field(resultado)))
    {
        tam_fila += columna->length + 1;
    }

    // Arreglo JSON con cada fila del resultado:
    json_object* arreglo = json_object_new_array(); // Arreglo con las tablas.
    MYSQL_ROW fila;
    int i = 0;
    while ((fila = mysql_fetch_row(resultado)))
    {
        char* fila_str = malloc(tam_fila);
        fila_str[0] = 0;
        for (int i = 0; i < cant_columnas; i++)
        {
            strcat(fila_str, fila[i]);
            strcat(fila_str, "\t\0");
        }
        fila_str[tam_fila] = 0;

        json_object* objeto_nombre_tabla = json_object_new_string(fila_str);
        json_object_array_add(arreglo, objeto_nombre_tabla);
        i++;
    }

    mysql_free_result(resultado); // Liberar resultado.

    // Regresar resultado como objeto JSON:
    json_object* objeto = json_object_new_object();
    json_object* objeto_cant_tablas = json_object_new_int(i);
    json_object_object_add(objeto, "cantidad", objeto_cant_tablas);
    json_object_object_add(objeto, "filas", arreglo);
    return objeto;
}
