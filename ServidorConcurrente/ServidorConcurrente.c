/* Comandos:
 * Cliente:  /run/SinTituloSockets.exe c
 * Servidor: /run/SinTituloSockets.exe
 * Servidor: /run/SinTituloSockets.exe s
 */

#include "cliente.h"
#include "servidor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constantes =========================================================
const char ARG_CLIENTE = 'c';
const char ARG_SERVIDOR = 's';

// Funciones ==========================================================
void enviar(char*);
void recibir(char*);


int main(int argc, char **argv)
{
    // Servidor ==================================
    if (argc == 1 ||
        argv[1][0] == ARG_SERVIDOR)
    {
        servidor();
        return 0;
    }

    // Cliente ===================================
    if (argv[1][0] == ARG_CLIENTE)
    {
        cliente(enviar, recibir);
        //~ cliente_uniq(enviar, recibir);

        printf(">>> ");
        getchar();
        return 0;
    }

    printf("Argumentos:\n");
    printf("%c:        Cliente\n", ARG_CLIENTE);
    printf("%c/<Nada>: Servidor\n", ARG_SERVIDOR);
    return 1;
}

void enviar(char* mensaje)
{
    printf("> Ingrese texto: ");
    fgets(mensaje, 256, stdin);
    mensaje[strlen(mensaje)] = 0;
    //~ printf("Para enviar: '%s'\n", mensaje);
}

void recibir(char* mensaje)
{
    printf("[Recibido]: %s\n", mensaje);
}
