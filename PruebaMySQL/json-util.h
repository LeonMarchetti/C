#ifndef JSONUTIL_H_
#define JSONUTIL_H_

// Librerías ==========================================================
#include <json.h>

// Tipos ==============================================================

// Constantes =========================================================

// Funciones ==========================================================
const char* json_get_string(json_object*, char*);
void print_json(json_object*);

#endif /* JSONUTIL_H_ */
