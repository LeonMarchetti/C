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

void importar_config();
void analizar_resultado(json_object* resultado);

void prueba_columnas(MYSQL*);
void prueba_consulta(MYSQL*);
void prueba_dbs(MYSQL*);

json_object* mysql_tablas(void*);
json_object* mysql_consulta(void* conexion, const char* consulta);
json_object* mysql_columnas(void* conexion, const char* tabla);
json_object* mysql_res_a_json(MYSQL_RES* resultado);
json_object* mysql_bases_de_datos(void* conexion);


int main(int argc, char **argv)
{
    importar_config();
    MYSQL* conexion = conectar();
    if (!conexion)
    {
        return 1;
    }

    //~ prueba_columnas(conexion);
    //~ prueba_consulta(conexion);
    prueba_dbs(conexion);

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

void importar_config()
{
    const char* formato = "%-15s|%s\n";

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
    printf(formato, "Host", host);

    base = json_get_string(objeto, KEY_BASE);
    if (!base)
    {
        printf("No se pudo leer el nombre de la base de datos (Clave: %s)...\n", KEY_BASE);
        exit(1);
    }
    printf(formato, "Base de datos", base);

    usuario = json_get_string(objeto, KEY_USER);
    if (!usuario)
    {
        printf("No se pudo leer el nombre de usuario (Clave: %s)...\n", KEY_USER);
        exit(1);
    }
    printf(formato, "Usuario", usuario);

    contrasena = json_get_string(objeto, KEY_PASS);
    if (!contrasena)
    {
        printf("No se pudo leer la contraseña (Clave: %s)...\n", KEY_PASS);
        exit(1);
    }
    //~ printf(formato, "Contraseña", contrasena);

    consulta = json_get_string(objeto, KEY_CONSULTA);
    if (!consulta)
    {
        printf("No se pudo leer la consulta (Clave: %s)...\n", KEY_CONSULTA);
        exit(1);
    }
    printf(formato, "Consulta", consulta);
    printf("\n");
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

void prueba_columnas(MYSQL* conexion)
{
    printf("Prueba Columnas...\n\n");
    json_object* resultado = mysql_columnas(conexion, "Factura");
    analizar_resultado(resultado);
    free(resultado);
}

void prueba_consulta(MYSQL* conexion)
{
    printf("Prueba Consulta...\n\n");
    json_object* resultado = mysql_consulta(conexion, consulta);
    analizar_resultado(resultado);
    free(resultado);
}

void prueba_dbs(MYSQL* conexion)
{
    printf("Prueba Bases de datos...\n\n");
    json_object* resultado = mysql_bases_de_datos(conexion);
    analizar_resultado(resultado);
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

json_object* mysql_bases_de_datos(void* conexion)
{
    MYSQL_RES* resultado = mysql_list_dbs(conexion, NULL);
    if (!resultado)
    {
        fprintf(stderr, "%s\n", mysql_error((MYSQL*) conexion));
        return NULL;
    }
    return mysql_res_a_json(resultado);
}
