#include "cliente.h"
#include "servidor.h"

// Librerías ==========================================================
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Constantes =========================================================
const int TAM_BUFFER_C = 30;

// Funciones ==========================================================
void cliente(void enviar(char*), void recibir(char*))
{
    struct sockaddr_in c_sock;
    int idsocks;
    int idsockc;
    int lensock;

    idsockc = socket(AF_INET, SOCK_STREAM, 0);

    c_sock.sin_family = AF_INET;
    c_sock.sin_port = htons(PUERTO);
    c_sock.sin_addr.s_addr = inet_addr(IP);

    lensock = sizeof(c_sock);

    // Conectarse con el servidor:
    idsocks = connect(idsockc, (struct sockaddr*) &c_sock, lensock);
    if (idsocks == -1)
    {
        printf("Falló el connect...\n");
        return;
    }

    int nb;
    char buf_in[TAM_BUFFER_C];
    char* buf_out;

    while (1)
    {
        // Preparar datos de salida:
        buf_out = malloc(256 * sizeof(char));
        enviar(buf_out);

        // Mandar al servidor:
        write(idsockc, buf_out, strlen(buf_out));
        //~ sleep(1);

        // Recibir del servidor:
        nb = read(idsockc, buf_in, TAM_BUFFER_C);
        buf_in[nb-1] = '\0';

        // Presionar solo enter para terminar la sesión
        if (!strcmp(buf_out, CERRAR))
        {
            break;
        }

        recibir(buf_in);

        free(buf_out);
    }
    close(idsockc);
}
