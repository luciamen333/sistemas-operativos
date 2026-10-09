#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void nada() {
    /* Manejador de señal vacío */
}

int main(int numero_argumentos, char *argumentos[]) {
    int max_profundidad, total_cadenas;
    int idx_cadena, idx_nivel;
    pid_t id_proceso;

    if (numero_argumentos == 3) {
        max_profundidad = atoi(argumentos[1]);
        total_cadenas = atoi(argumentos[2]);

        if (max_profundidad > 0 && total_cadenas > 0) {

            for (idx_cadena = 1; idx_cadena <= total_cadenas; idx_cadena++) {
                id_proceso = fork();

                if (id_proceso == 0) {
                    // Proceso hijo (inicio de una nueva cadena)
                    for (idx_nivel = 1; idx_nivel <= max_profundidad - 1; idx_nivel++) {
                        id_proceso = fork();

                        if (id_proceso != 0) {
                            // Proceso padre en el nivel actual: espera al siguiente subproceso
                            wait(NULL);
                            exit(0);
                        }
                    }

                    // Se alcanza el último nivel de la cadena
                    if (idx_nivel == max_profundidad) {
                        signal(SIGALRM, nada);
                        alarm(10);
                        pause();
                        exit(0);
                    }
                }
            }

            // El proceso principal espera a que finalicen todas las cadenas creadas
            if (idx_cadena == total_cadenas + 1) {
                int k;
                for (k = 1; k <= total_cadenas; k++) {
                    wait(NULL);
                }
            }
        }
    }

    return 0;
}