// Librerías ==========================================================
#include <json.h>
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json-util.h"

// Constantes =========================================================
#define ARCHIVO      "config.json"
#define KEY_CONSULTA "sql"
#define KEY_HOST     "host"
#define KEY_BASE     "base"
#define KEY_USER     "usuario"
#define KEY_PASS     "contraseña"

// Globales ===========================================================
// Servidor MySQL:
const char* host;
const char* base;
const char* usuario;
const char* contrasena;
const char* consulta;

// Funciones ==========================================================
MYSQL* conectar();

void consultar(MYSQL*, const char*);
void importar_consulta();
void importar_config();

void prueba_consulta(MYSQL*);
void prueba_columnas(MYSQL*);

void separador();

json_object* mysql_tablas(void*);
json_object* mysql_consulta(void* conexion, const char* consulta);
json_object* mysql_columnas(void* conexion, const char* tabla);
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
    //~ prueba_consulta(conexion);
    prueba_columnas(conexion);
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
    printf("Host:          '%s'\n", host);

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
    separador();
}

void prueba_consulta(MYSQL* conexion)
{
    json_object* resultado = mysql_consulta(conexion, consulta);
    //~ json_object* resultado = mysql_consulta(conexion, CONSULTA);
    //~ json_object* resultado = mysql_tablas(conexion);

    if (!resultado)
    {
        fprintf(stderr, "No se obtuvieron resultados\n");
        return;
    }


    json_object* objeto_cantidad;
    json_object_object_get_ex(resultado, "cantidad", &objeto_cantidad);
    if (json_object_get_int(objeto_cantidad) <= 0)
    {
        printf("No hay resultado para mostrar...\n");
        return;
    }
    printf("Filas: %d\n", json_object_get_int(objeto_cantidad));

    // Mostrar nombres de columnas ============================================
    json_object* arr_columnas;
    json_object_object_get_ex(resultado, "columnas", &arr_columnas);

    int          i = 0;
    json_object* obj_columna;

    printf("Fila\t");
    while ((obj_columna = json_object_array_get_idx(arr_columnas, i++)))
    {
        printf("|%s\t", json_object_get_string(obj_columna));
    }
    printf("\n----------------------------------------\n");

    // Iterar filas:
    json_object* arr_filas;
    json_object_object_get_ex(resultado, "filas", &arr_filas);

    int          f = 0;
    json_object* arr_fila;
    while ((arr_fila = json_object_array_get_idx(arr_filas, f++)))
    {
        printf("%d\t", f);
        int          c = 0;
        json_object* obj_celda;
        while ((obj_celda = json_object_array_get_idx(arr_fila, c++)))
        {
            printf("|%s\t", json_object_get_string(obj_celda));
        }
        printf("\n");
    }

    print_json(resultado);
    free(resultado);
}

void prueba_columnas(MYSQL* conexion)
{
    printf("Prueba Columnas...\n");
    json_object* resultado = mysql_columnas(conexion, "Factura");
    if (!resultado)
    {
        fprintf(stderr, "No se obtuvieron resultados\n");
        return;
    }

    json_object* obj_cantidad;
    json_object* obj_columnas;
    json_object* obj_filas;

    json_object_object_get_ex(resultado, "cantidad", &obj_cantidad);
    json_object_object_get_ex(resultado, "columnas", &obj_columnas);
    json_object_object_get_ex(resultado, "filas",    &obj_filas);

    if (json_object_get_int(obj_cantidad) <= 0)
    {
        printf("No hay resultado para mostrar...\n");
        return;
    }
    printf("Filas: %d\n", json_object_get_int(obj_cantidad));

    // Mostrar nombres de columnas ============================================
    int          i = 0;
    json_object* obj_columna;
    printf("Fila\t");
    while ((obj_columna = json_object_array_get_idx(obj_columnas, i++)))
    {
        printf("|%s\t", json_object_get_string(obj_columna));
    }
    printf("\n");
    separador();

    // Iterar filas:
    int          f = 0;
    json_object* arr_fila;
    while ((arr_fila = json_object_array_get_idx(obj_filas, f++)))
    {
        printf("%d\t", f);
        int          c = 0;
        json_object* obj_celda;
        while ((obj_celda = json_object_array_get_idx(arr_fila, c++)))
        {
            printf("|%s\t", json_object_get_string(obj_celda));
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

json_object* mysql_columnas(void* conexion, const char* tabla)
{
    const char* plantilla_sql = "Show Columns From %s";
    char*       consulta      = malloc(strlen(tabla) + strlen(plantilla_sql));

    sprintf(consulta, plantilla_sql, tabla);
    printf("'%s'\n", consulta);
    return mysql_consulta(conexion, consulta);
}

json_object* mysql_res_a_json(MYSQL_RES* resultado)
{
    // Longitud de string para cada fila:
    int          cant_columnas    = mysql_num_fields(resultado);
    json_object* arreglo_columnas = json_object_new_array();
    MYSQL_FIELD* columna;
    while ((columna = mysql_fetch_field(resultado)))
    {
        json_object_array_add(
            arreglo_columnas,
            json_object_new_string(columna->name));
    }

    // Arreglo con cada fila del resultado:
    json_object* arreglo = json_object_new_array(); // Arreglo con las tablas.
    int          i       = 0;
    MYSQL_ROW    fila;
    while ((fila = mysql_fetch_row(resultado)))
    {
        // Arreglo JSON con cada valor de la fila:
        json_object* arreglo_fila = json_object_new_array();
        for (int c = 0; c < cant_columnas; c++)
        {
            json_object_array_add(
                arreglo_fila,
                json_object_new_string(
                    (fila[c]) ? fila[c] : "NULL"));
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

void separador()
{
    char SEPARADOR[80];
    memset(SEPARADOR, '-', 79);
    SEPARADOR[78] = '\n';
    SEPARADOR[79] = 0;
    printf(SEPARADOR);
}
