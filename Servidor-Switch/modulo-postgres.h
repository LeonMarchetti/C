#ifndef POSTGRES_H_
#define POSTGRES_H_

// Librerías ==========================================================
#include <json.h>

// Tipos ==============================================================

// Constantes =========================================================

// Funciones ==========================================================
/** Realiza una conexión a una base de datos PostgreSQL.
 *
 * @param host Dirección del servidor.
 * @param puerto Número de puerto del servidor.
 * @param usuario Nombre de usuario para la conexión.
 * @param contrasenia Contraseña para la conexión.
 * @param nombre_bd Nombre de la base de datos a conectarse.
 * @returns Puntero a una estructura PostgreSQL.
 */
void* postgres_conectar(const char* host, int puerto, const char* usuario,
                        const char* contrasenia, const char* nombre_bd);

/** Cierra la conexión con la base de datos PostgreSQL.
 *
 * @param conexion Puntero a una estructura PostgreSQL
 */
void postgres_cerrar(void* conexion);

/** Realiza una consulta a una base de datos MySQL.
 *
 * @param conexion Puntero a una estructura PostgreSQL
 * @param consulta Consulta de SQL.
 * @returns Objeto JSON, con un entero para la cantidad de filas en el
 * resultado y un arreglo con las filas del resultado.
 */
json_object* postgres_consulta(void* conexion, const char* consulta);

/** Regresa los nombres de todas las tablas de la base de datos MySQL actual.
 *
 * @param conexion Puntero a una estructura PostgreSQL
 * @returns Objeto JSON, con un entero para la cantidad de tablas en el
 * resultado y un arreglo con los nombres de las tablas.
 */
json_object* postgres_tablas(void* conexion);

#endif /* POSTGRES_H_ */
