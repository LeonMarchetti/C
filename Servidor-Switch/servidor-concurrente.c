// Librerías ==========================================================
#include "servidor-concurrente.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Constantes =========================================================
const int BUFFER = 256;
const char* CERRAR = "\n";

// Funciones ==========================================================
void* atender(void *arg)
{
    // Para que el hilo termine cuando termine la función:
    pthread_detach(pthread_self());

    char buf_in[BUFFER];
    char buf_out[BUFFER];
    int nb;

    // Recibo los datos pasados al hilo por el puntero:
    struct datos_t *datos = (struct datos_t*) arg;

    printf("[Hilo][%d] Atendiendo socket\n", datos->socket);

    while (1)
    {
        // Recibir datos:
        nb = read(datos->socket, buf_in, BUFFER);
        buf_in[nb] = '\0';

        // Presionar solo enter para terminar la sesión
        if (!strcmp(buf_in, CERRAR))
        {
            break;
        }
        buf_in[nb-1] = '\0'; // Saco el salto de línea
        sprintf(buf_out, "Recibido \"%s\"", buf_in);

        // Mostrar en pantalla:
        printf("[Hilo][%d] %s\n", datos->socket, buf_out);

        // Enviar datos:
        write(datos->socket, buf_out, BUFFER);
    }

    // Cerrar socket:
    close(datos->socket);
    printf("[Hilo][%d] Socket cerrado\n", datos->socket);

    free(datos);
    return NULL;
}

void cliente(const char* host, int puerto)
{
    struct sockaddr_in c_sock;
    int idsocks;
    int idsockc;
    int lensock;

    idsockc = socket(AF_INET, SOCK_STREAM, 0);

    c_sock.sin_family = AF_INET;
    c_sock.sin_port = htons(puerto);
    c_sock.sin_addr.s_addr = inet_addr(host);

    lensock = sizeof(c_sock);

    // Conectarse con el servidor:
    idsocks = connect(idsockc, (struct sockaddr*) &c_sock, lensock);
    if (idsocks == -1)
    {
        printf("Falló el connect...\n");
        return;
    }

    int nb;
    char buf_in[BUFFER];
    char buf_out[BUFFER];

    while (1)
    {
        // Entrada por consola:
        printf("> Ingrese texto: ");
        fgets(buf_out, BUFFER, stdin);

        // Mandar al servidor:
        write(idsockc, buf_out, strlen(buf_out));
        //~ sleep(1);

        // Recibir del servidor:
        nb = read(idsockc, buf_in, BUFFER);
        buf_in[nb-1] = '\0';

        // Presionar solo enter para terminar la sesión
        if (!strcmp(buf_out, CERRAR))
        {
            break;
        }

        // Mostrar en pantalla:
        printf("[%d] > %s\n", idsockc, buf_in);
    }
    close(idsockc);
}

void servidor(const char* host, int puerto)
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
    s_sock.sin_port = htons(puerto);
    s_sock.sin_addr.s_addr = inet_addr(host);

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
