// Librerías ==========================================================
#include "json-util.h"

#include <json.h>
#include <stdio.h>

// Constantes =========================================================

// Funciones ==========================================================
const char* json_get_string(json_object* objeto, char* clave)
{
    json_object* objeto_hijo;
    json_object_object_get_ex(objeto, clave, &objeto_hijo);
    return json_object_get_string(objeto_hijo);
}

void print_json(json_object* objeto)
{
    printf("========================================\n");
    printf("String JSON:\n");
    printf("%s\n", json_object_to_json_string_ext(objeto, 0));
    printf("========================================\n");
}
