#ifndef SERVIDOR_H_
#define SERVIDOR_H_

// Tipos ==============================================================
struct datos_t
{
    int socket;
};

// Constantes =========================================================
extern const char* CERRAR;

// Funciones ==========================================================
void* atender(void*);
void cliente(const char*, int);
void servidor(const char*, int);

#endif /* SERVIDOR_H_ */
