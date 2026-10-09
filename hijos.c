#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

void despierta(int sig) {}

void crearMemoriaCompartida(int x, int y, int **pidsX, int **pidsY, int *shm_x, int *shm_y) {
    *shm_x = shmget(IPC_PRIVATE, sizeof(int) * x, IPC_CREAT | 0666);
    *pidsX = (int *) shmat(*shm_x, 0, 0);

    *shm_y = shmget(IPC_PRIVATE, sizeof(int) * y, IPC_CREAT | 0666);
    *pidsY = (int *) shmat(*shm_y, 0, 0);
}

void liberarMemoriaCompartida(int *pidsX, int *pidsY, int shm_x, int shm_y) {
    shmdt(pidsX);
    shmdt(pidsY);
    shmctl(shm_x, IPC_RMID, NULL);
    shmctl(shm_y, IPC_RMID, NULL);
}

void ejecutarAbanicoHijosY(int y, int *pidsY) {
    int i;
    pid_t pid;

    for (i = 1; i <= y; i++) {
        pid = fork();
        if (pid == 0) {
            pidsY[i - 1] = getpid();
            signal(SIGALRM, despierta);
            alarm(10);
            pause();
            exit(0);
        }
    }

    for (i = 1; i <= y; i++) {
        wait(NULL);
    }
}

int crearCadenaProcesosX(int x, pid_t pidPadre, int *pidsX) {
    int i;
    pid_t pid;

    for (i = 1; i <= x; i++) {
        pid = fork();
        if (pid != 0) {
            wait(NULL);
            break;
        } else {
            pidsX[i - 1] = getpid();
            printf("Soy el proceso %d. Mis padres son: %d", getpid(), pidPadre);
            for (int j = 0; j < i - 1; j++) {
                printf(", %d", pidsX[j]);
            }
            printf("\n");
        }
    }
    return i;
}

void mostrarResultadosPadre(int y, pid_t pidPadre, int *pidsY, int *pidsX, int shm_x, int shm_y) {
    printf("Soy el super padre %d, mis hijos finales son: ", pidPadre);
    for (int i = 0; i < y; i++) {
        printf("%d", pidsY[i]);
        if (i != y - 1) {
            printf(", ");
        }
    }
    printf("\n");

    liberarMemoriaCompartida(pidsX, pidsY, shm_x, shm_y);
}

int main(int argc, char *argv[]) {
    int x, y, shm_x, shm_y;
    int *pidsX, *pidsY;
    pid_t pidPadre;

    if (argc != 3) {
        printf("Error en argumentos\n");
        return 1;
    }

    pidPadre = getpid();
    x = atoi(argv[1]);
    y = atoi(argv[2]);

    crearMemoriaCompartida(x, y, &pidsX, &pidsY, &shm_x, &shm_y);

    int nivelAlcanzado = crearCadenaProcesosX(x, pidPadre, pidsX);

    if (nivelAlcanzado == 1) {
        mostrarResultadosPadre(y, pidPadre, pidsY, pidsX, shm_x, shm_y);
    } else if (nivelAlcanzado == x + 1) {
        ejecutarAbanicoHijosY(y, pidsY);
    }

    return 0;
}