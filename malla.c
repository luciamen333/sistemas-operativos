#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void nada() {
   
}

int main(int numero_argumentos, char *argumentos[]) {
    int max_filas, max_columnas;
    int idx_columna, idx_fila;
    pid_t id_proceso;

    if (numero_argumentos != 3) {
        return 1;
    }

    max_filas = atoi(argumentos[1]);      
    max_columnas = atoi(argumentos[2]);   

    if (max_filas <= 0 || max_columnas <= 0) {
        return 1;
    }

    
    signal(SIGALRM, nada);

    for (idx_columna = 1; idx_columna <= max_columnas; idx_columna++) {
        id_proceso = fork();

        if (id_proceso == 0) {
            
            
            for (idx_fila = 2; idx_fila <= max_filas; idx_fila++) {
                id_proceso = fork();

                if (id_proceso == 0) {
                   
                    continue;
                } else {
                    
                    break;
                }
            }

           
            alarm(15);
            pause();

            if (id_proceso > 0) {
                wait(NULL);
            }

            exit(0);
        }
    }

    
    for (idx_columna = 1; idx_columna <= max_columnas; idx_columna++) {
        wait(NULL);
    }

    return 0;
}