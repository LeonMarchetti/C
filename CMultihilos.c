#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TAM_BUFFER 256

struct datos_t {
    int numero;
    char mensaje[TAM_BUFFER];
};

void *callback(void *arg) {
    sleep(1);

    struct datos_t *datos = (struct datos_t*) arg;
    printf("[Hilo] Mensaje: '%s'\n", datos->mensaje);
    printf("[Hilo] Numero : '%d'\n", datos->numero);

    return NULL;
}

int main(int argc, char **argv) {
    pthread_t id_hilo;
    printf("Antes del hilo.\n");

    srand(time(NULL));

    struct datos_t est;
    est.numero = rand();
    strcpy(est.mensaje, "Hola mundo");

    printf("[Main] Mensaje: '%s'\n", est.mensaje);
    printf("[Main] Numero : '%d'\n", est.numero);

    // Ejecución del hilo =====================================================
    // Se devuelve el identificador del hilo en "id_hilo"
    int estado = pthread_create(&id_hilo, NULL, callback, &est);

    if (estado != 0) {
        printf("Creación de hilo falló con error: %d\n", estado);

    } else {
        pthread_join(id_hilo, NULL);
        printf("Después del hilo.\n");
    }
}
