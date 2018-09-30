/* Comandos:
 * Cliente:  /run/SinTituloSockets.exe c
 * Servidor: /run/SinTituloSockets.exe
 * Servidor: /run/SinTituloSockets.exe s
 */

#include "cliente.h"
#include "servidor.h"

#include <stdio.h>

// Constantes =========================================================
const char ARG_CLIENTE = 'c';
const char ARG_SERVIDOR = 's';

// Funciones ==========================================================
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
        cliente();
        getchar();
        return 0;
    }

    printf("Argumentos:\n");
    printf("%c:        Cliente\n", ARG_CLIENTE);
    printf("%c/<Nada>: Servidor\n", ARG_SERVIDOR);
    return 1;
}
