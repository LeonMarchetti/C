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

void prueba_consulta(MYSQL*);

json_object* mysql_tablas(void*);
json_object* mysql_consulta(void* conexion, const char* consulta);
json_object* mysql_res_a_json(MYSQL_RES* resultado);

int main(int argc, char **argv)
{
    importar_config();
    MYSQL* conexion = conectar();
    if (!conexion)
    {
        return 1;
    }

    //~ consultar(conexion, CONSULTA);
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
    //~ if (mysql_query(conexion, consulta))
    //~ {
        //~ fprintf(stderr, "%s\n", mysql_error(conexion));
        //~ printf("Error en la consulta:\n%s\n", mysql_error(conexion));
        //~ exit(1);
    //~ }

    // Obtener resultado de la consulta:
    //~ MYSQL_RES* resultado = mysql_use_result(conexion);
    //~ MYSQL_RES* resultado = mysql_store_result(conexion);

    MYSQL_RES* resultado = mysql_list_tables(conexion, NULL);

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

void prueba_consulta(MYSQL* conexion)
{
    //~ json_object* resultado = mysql_consulta(conexion, CONSULTA);
    json_object* resultado = mysql_tablas(conexion);

    if (!resultado)
    {
        fprintf(stderr, "No se obtuvieron resultados\n");
        return;
    }

    json_object* objeto_cantidad;
    json_object_object_get_ex(resultado, "cantidad", &objeto_cantidad);
    int cant_filas = json_object_get_int(objeto_cantidad);
    if (cant_filas <= 0)
    {
        printf("Resultado vacío\n");
        return;
    }
    printf("Filas: %d\n", cant_filas);

    // Columnas:
    json_object* arreglo_columnas;
    json_object_object_get_ex(resultado, "columnas", &arreglo_columnas);
    int i = 0;
    json_object* obj_columna;
    printf("Fila\t");
    while ((obj_columna = json_object_array_get_idx(arreglo_columnas, i++)))
    {
        printf("|%s\t", json_object_get_string(obj_columna));
    }
    printf("\n");
    printf("----------------------------------------\n");

    // Iterar filas:
    json_object* obj_filas;
    json_object_object_get_ex(resultado, "filas", &obj_filas);
    for (int f = 0; f < cant_filas; f++)
    {
        printf("%d\t", f);

        json_object* obj_fila    = json_object_array_get_idx(obj_filas, f);
        int          cant_celdas = json_object_array_length(obj_fila);

        // Iterar celdas:
        for (int c = 0; c < cant_celdas; c++)
        {
            json_object* obj_celda = json_object_array_get_idx(obj_fila, c);
            const char* celda = json_object_get_string(obj_celda);
            printf("|%s\t", celda);
        }
        printf("\n");
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

    return mysql_res_a_json(resultado);
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

    return mysql_res_a_json(resultado);
}

json_object* mysql_res_a_json(MYSQL_RES* resultado)
{
    // Longitud de string para cada fila:
    int          cant_columnas    = mysql_num_fields(resultado);
    int          tam_fila         = 0;
    json_object* arreglo_columnas = json_object_new_array();
    MYSQL_FIELD* columna;
    while ((columna = mysql_fetch_field(resultado)))
    {
        json_object_array_add(
            arreglo_columnas,
            json_object_new_string(columna->name));

        tam_fila += columna->length + 1;
    }

    // Arreglo con cada fila del resultado:
    json_object* arreglo = json_object_new_array(); // Arreglo con las tablas.
    MYSQL_ROW    fila;
    int          i       = 0;
    while ((fila = mysql_fetch_row(resultado)))
    {
        // Arreglo JSON con cada valor de la fila:
        json_object* arreglo_fila = json_object_new_array();
        for (int c = 0; c < cant_columnas; c++)
        {
            json_object_array_add(
                arreglo_fila,
                json_object_new_string(fila[c])
            );
        }
        json_object_array_add(arreglo, arreglo_fila);
        i++;
    }

    mysql_free_result(resultado); // Liberar resultado.

    // Regresar resultado como objeto JSON:
    json_object* objeto            = json_object_new_object();
    json_object* objeto_cant_filas = json_object_new_int(i);

    json_object_object_add(objeto, "cantidad", objeto_cant_filas);
    json_object_object_add(objeto, "columnas", arreglo_columnas);
    json_object_object_add(objeto, "filas", arreglo);
    return objeto;
}
