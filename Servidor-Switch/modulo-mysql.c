// Librerías ==================================================================
#include "modulo-mysql.h"

#include <json.h>
#include <mysql.h>
#include <stdio.h>
#include <string.h>

// Constantes =================================================================
const char* JSON_KEY_CANTIDAD = "cantidad";
const char* JSON_KEY_FILAS    = "filas";

// Funciones ==================================================================
void* mysql_conectar(const char* host, int puerto, const char* usuario,
                     const char* contrasenia, const char* nombre_bd)
{
    MYSQL* conexion = mysql_init(NULL);
    if (!mysql_real_connect(conexion, host, usuario, contrasenia, nombre_bd, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conexion));
        return NULL;
    }
    return conexion;
}

void mysql_cerrar(void* conexion)
{
    mysql_close((MYSQL*) conexion);
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
    int          tam_fila = 0;
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
    json_object_object_add(objeto, JSON_KEY_CANTIDAD, objeto_cant_tablas);
    json_object_object_add(objeto, JSON_KEY_FILAS, arreglo);
    return objeto;
}

json_object* mysql_tablas(void* conexion)
{
    MYSQL_RES* resultado = mysql_list_tables((MYSQL*) conexion, NULL);
    if (!resultado)
    {
        fprintf(stderr, "%s\n", mysql_error((MYSQL*) conexion));
        return NULL;
    }

    int cant_tablas      = mysql_num_rows(resultado); // Cantidad de tablas.
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
    json_object_object_add(objeto, JSON_KEY_CANTIDAD, objeto_cant_tablas);
    json_object_object_add(objeto, JSON_KEY_FILAS, arreglo);
    return objeto;
}
