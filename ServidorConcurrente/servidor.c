#include "servidor.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

// Constantes =========================================================
const char* IP = "127.0.0.1";
const int PUERTO = 8080;
const int TAM_BUFFER_S = 30;

// Funciones ==========================================================
void *atender(void *arg)
{
    // Para que el hilo termine cuando termine la función:
    pthread_detach(pthread_self());

    char buf[TAM_BUFFER_S];
    char resp[TAM_BUFFER_S];
    int nb;

    // Recibo los datos pasados al hilo por el puntero:
    struct datos_t *datos = (struct datos_t*) arg;

    printf("[Hilo][%d] Atendiendo socket\n", datos->socket);

    // Recibir datos:
    nb = read(datos->socket, buf, TAM_BUFFER_S);
    buf[nb] = '\0';

    // Mostrar en pantalla:
    printf("[Hilo][%d] %s", datos->socket, buf);

    // Enviar datos:
    sprintf(resp, "Recibido\n");
    write(datos->socket, resp, TAM_BUFFER_S);

    // Cerrar socket:
    close(datos->socket);
    printf("[Hilo][%d] Socket cerrado\n", datos->socket);

    free(datos);
    return NULL;
}

void servidor()
{
    struct sockaddr_in s_sock;
    struct sockaddr_in c_sock;
    int idsocks;
    int idsockc;
    int lensock;

    // Estructura con los datos a pasar al hilo
    struct datos_t *datos;

    idsocks = socket(AF_INET, SOCK_STREAM, 0);

    s_sock.sin_family = AF_INET;
    s_sock.sin_port = htons(PUERTO);
    s_sock.sin_addr.s_addr = inet_addr(IP);

    lensock = sizeof(struct sockaddr_in);
    bind(idsocks, (struct sockaddr*) &s_sock, lensock);

    listen(idsocks, 5);

    lensock = sizeof(c_sock);

    while (1)
    {
        printf("[Main]    Esperando conexión...\n");

        // Acepto una conexión entrante:
        idsockc = accept(idsocks, (struct sockaddr*) &c_sock, &lensock);
        if (idsockc == -1)
        {
            printf("[Main]    Falló el accept\n");
            continue;
        }

        // Derivar conexión a un hilo:
        printf("[Main][%d] Derivando conexión de socket\n", idsockc);

        // Inicializo la estructura. Luego la libero en el hilo:
        datos = malloc(sizeof(struct datos_t));
        datos->socket = idsockc;

        // Iniciar hilo:
        pthread_t id_hilo;
        pthread_create(&id_hilo, NULL, atender, datos);
    }
}
