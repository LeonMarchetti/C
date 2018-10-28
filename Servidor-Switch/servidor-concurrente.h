#ifndef SERVIDOR_H_
#define SERVIDOR_H_

// Tipos ==============================================================
struct datos_t
{
    int socket;
};

// Constantes =========================================================
extern const char* CERRAR;
extern const int   BUFFER;

// Funciones ==========================================================
void cliente(const char*, int);
void servidor(const char*, int, void*);

#endif /* SERVIDOR_H_ */
