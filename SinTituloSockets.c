// Cliente:  /run/SinTituloSockets.exe c
// Servidor: /run/SinTituloSockets.exe
// Servidor: /run/SinTituloSockets.exe s
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// Constantes:
const char ARG_CLIENTE = 'c';
const char ARG_SERVIDOR = 's';

const int BUFFER_ENTRADA = 30;
const char* IP = "127.0.0.1";
const int PUERTO = 8000;
const int TAM_BUFFER = 30;

// Funciones:
void servidor();
void cliente();

int main(int argc, char **argv) {

    // Servidor ==================================
    if (argc == 1 ||
        argv[1][0] == ARG_SERVIDOR)
    {
        servidor();
        return 0;
    }

    // Cliente ===================================
    if (argv[1][0] == ARG_CLIENTE) {
        cliente();
        return 0;
    }

    printf("Argumentos:\n");
    printf("%c:        Cliente\n", ARG_CLIENTE);
    printf("%c/<Nada>: Servidor\n", ARG_SERVIDOR);
    return 1;
}

void servidor() {
    srand(time(NULL));
    int n = rand() % 10;
    printf("Servidor n°%d\n", n);

    struct sockaddr_in s_sock;
    struct sockaddr_in c_sock;
    int idsocks;
    int idsockc;
    int lensock;

    idsocks = socket(AF_INET, SOCK_STREAM, 0);

    s_sock.sin_family = AF_INET;
    s_sock.sin_port = htons(PUERTO);
    s_sock.sin_addr.s_addr = inet_addr(IP);

    lensock = sizeof(struct sockaddr_in);
    bind(idsocks, (struct sockaddr*) &s_sock, lensock);

    listen(idsocks, 5);

    lensock = sizeof(c_sock);

    char SEPARADOR[80];
    memset(SEPARADOR, '=', 79);

    while (1) {
        printf("%s\nEsperando conexión...\n", SEPARADOR);

        idsockc = accept(idsocks, (struct sockaddr*) &c_sock, &lensock);
        if (idsockc == -1) {
            printf("Falló el accept\n");

        } else {
            char buf[TAM_BUFFER];
            char resp[TAM_BUFFER];
            int nb;

            printf("Conexión aceptada\n");

            // Recibir datos:
            nb = read(idsockc, buf, TAM_BUFFER);
            buf[nb] = '\0';

            // Mostrar en pantalla:
            printf("[%d] > %s", idsockc, buf);

            // Enviar datos:
            sprintf(resp, "Recibido <%d>", n);
            write(idsockc, resp, TAM_BUFFER);

            // Cerrar socket:
            close(idsockc);
        }
    }
}

void cliente() {
    srand(time(NULL));

    struct sockaddr_in c_sock;
    int idsocks;
    int idsockc;
    int lensock;

    idsockc = socket(AF_INET, SOCK_STREAM, 0);

    c_sock.sin_family = AF_INET;
    c_sock.sin_port = htons(PUERTO);
    c_sock.sin_addr.s_addr = inet_addr(IP);

    lensock = sizeof(c_sock);

    idsocks = connect(idsockc, (struct sockaddr*) &c_sock, lensock);
    if (idsocks == -1) {
        printf("Falló el connect...\n");

    } else {
        int nb;
        char buf[TAM_BUFFER];

        // Entrada por consola:
        printf("> Ingrese texto: ");
        fgets(buf, TAM_BUFFER, stdin);

        // Mandar al servidor:
        write(idsockc, buf, strlen(buf));
        sleep(1);

        // Recibir del servidor:
        nb = read(idsockc, buf, TAM_BUFFER);
        buf[nb] = '\0';

        // Mostrar en pantalla:
        printf("[%d] > %s\n", idsockc, buf);
        close(idsockc);
    }
}
