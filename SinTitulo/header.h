#ifndef HEADER_H_
#define HEADER_H_

// Librerías ==========================================================
#include <pthread.h>
#include <stdio.h>

// Constantes =========================================================

// Funciones ==========================================================
pthread_t iniciar_hilo_terminar();
char* leer_archivo(FILE*);

#endif /* HEADER_H_ */
