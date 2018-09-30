#include "cliente.h"
#include "servidor.h"

// Librerías ==========================================================
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Constantes =========================================================
const int TAM_BUFFER_C = 30;

// Funciones ==========================================================
void cliente()
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
    char buf[TAM_BUFFER_C];

    // Entrada por consola:
    printf("> Ingrese texto: ");
    fgets(buf, TAM_BUFFER_C, stdin);

    // Mandar al servidor:
    write(idsockc, buf, strlen(buf));
    sleep(1);

    // Recibir del servidor:
    nb = read(idsockc, buf, TAM_BUFFER_C);
    buf[nb] = '\0';

    // Mostrar en pantalla:
    printf("[%d] > %s\n", idsockc, buf);
    close(idsockc);
}
