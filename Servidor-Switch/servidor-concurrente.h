#ifndef SERVIDOR_H_
#define SERVIDOR_H_

// Tipos ==============================================================
struct datos_t {
    int socket;
};

// Funciones ==========================================================
void* atender(void*);
void servidor(const char*, int);

#endif /* SERVIDOR_H_ */
