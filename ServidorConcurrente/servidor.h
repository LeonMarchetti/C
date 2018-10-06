#ifndef SERVIDOR_H_
#define SERVIDOR_H_

// Tipos ==============================================================
struct datos_t
{
    int socket;
};

// Constantes =========================================================
// Ubicación del servidor:
extern const char* IP;
extern const int PUERTO;

extern const char* CERRAR;

// Funciones ==========================================================
void *atender(void*);
void servidor();

#endif /* SERVIDOR_H_ */
