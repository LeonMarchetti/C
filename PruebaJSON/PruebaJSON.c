// Librerías ==========================================================
#include <json.h>
#include <stdio.h>

// Constantes =========================================================
const char* ARCHIVO = "ejemplo.json";

// Funciones ==========================================================
void         prueba_importar_objeto();
void         prueba_nuevo_objeto();
json_object* importar_de_archivo();
json_object* importar_de_string();
const char*  json_get_string(json_object* objeto, char* clave);
void         prueba_importar_atributo_inexistente();


int main(int argc, char **argv)
{
    //~ prueba_importar_objeto();
    prueba_importar_atributo_inexistente();
    //~ prueba_nuevo_objeto();
}

void prueba_importar_objeto()
{

    json_object* objeto_json = importar_de_archivo();
    //~ json_object* objeto_json = importar_de_string();

    // Validar objeto json:
    if (!objeto_json)
    {
        printf("No es un string JSON válido");
        exit(1);
    }

    // Cantidad de campos de objeto json:
    printf("Longitud: %d\n", json_object_object_length(objeto_json));

    // Listar objetos hijos:
    /*
    json_object_object_foreach(objeto_json, clave, valor)
    {
        // valor es json_object*
        // clave es char*
        printf("clave: %s\t", clave);
        switch (json_object_get_type(valor))
        {
            case json_type_string:
                printf("valor: %s\n", json_object_get_string(valor));
                break;
            case json_type_int:
                printf("valor: %d\n", json_object_get_int(valor));
                break;
            case json_type_double:
                printf("valor: %.2f\n", json_object_get_double(valor));
                break;
            case json_type_array:
                printf("valor:\n[\n");
                int n = json_object_array_length(valor);
                for (int i = 0; i < n; i++)
                {
                    json_object* e = json_object_array_get_idx(valor, i);
                    const char* valor_e = json_object_get_string(e);
                    printf("\t%s,\n", valor_e);
                }
                printf("]\n");
                break;
            default:
                printf("tipo desconocido\n");
        }
    }
    */

    json_object_object_foreach(objeto_json, clave, valor)
    {
        const char* valor_str = json_object_get_string(valor);
        printf("clave: %s\t%s\n", clave, valor_str);
    }

    printf("\n");

    json_object* objeto_basededatos;
    json_object* objeto_sql;
    json_bool b_basededatos = json_object_object_get_ex(objeto_json, "basededatos", &objeto_basededatos);
    json_bool b_sql = json_object_object_get_ex(objeto_json, "sql", &objeto_sql);

    if (b_basededatos)
    {
        printf("Base de datos:\t%s\n", json_object_get_string(objeto_basededatos));
    }

    if (b_sql)
    {
        printf("Consulta:\t%s\n", json_object_get_string(objeto_sql));
    }

    printf("\nJSON:\n%s\n", json_object_to_json_string(objeto_json));
}

json_object* importar_de_string()
{
    char* contenido_json = "{ \"basededatos\": \"facturacion\", \"sql\": \"Select * From Factura\" }";
    return json_tokener_parse(contenido_json);
}

json_object* importar_de_archivo()
{
    return json_object_from_file(ARCHIVO);
}

void prueba_nuevo_objeto()
{
    // Crear raíz:
    json_object* objeto = json_object_new_object();

    // Crear valores:
    json_object* string = json_object_new_string("Hola Mundo");
    json_object* numero = json_object_new_int(121899);

    // Agregar valores al objeto:
    json_object_object_add(objeto, "string", string);
    json_object_object_add(objeto, "numero", numero);

    // Mostrar:
    printf("%s\n", json_object_to_json_string(objeto));
}

const char* json_get_string(json_object* objeto, char* clave)
{
    json_object* objeto_hijo;
    json_object_object_get_ex(objeto, clave, &objeto_hijo);
    return json_object_get_string(objeto_hijo);
}

void prueba_importar_atributo_inexistente()
{
    json_object* objeto_json = importar_de_archivo();
    const char* atributo = json_get_string(objeto_json, "blah");
    if (atributo)
    {
        printf("atributo: '%s'\n", atributo);
    }
    else
    {
        printf("El atributo 'blah' no existe...\n");
    }
}
