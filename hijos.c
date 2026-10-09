#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void despierta() {}

int main(int argc, char *argv[]) {
    int i, x, y, shm_x, shm_y;
    int *pidsX, *pidsY;
    pid_t pid, pidPadre;

    if (argc != 3) {
        printf("Error en argumentos\n");
    } else {
        pidPadre = getpid();
        x = atoi(argv[1]);
        y = atoi(argv[2]);

        shm_x = shmget(IPC_PRIVATE, sizeof(int) * x, IPC_CREAT | 0666);
        pidsX = (int *) shmat(shm_x, 0, 0);

        shm_y = shmget(IPC_PRIVATE, sizeof(int) * y, IPC_CREAT | 0666);
        pidsY = (int *) shmat(shm_y, 0, 0);

        for (i = 1; i <= x; i++) {
            pid = fork();
            if (pid != 0) {
                wait(NULL);
                break;
            } else {
                pidsX[i - 1] = getpid();
                printf("Soy el proceso %d. Mis padres son: ", getpid());
                printf("%d", pidPadre);

                for (int j = 0; j < i - 1; j++) {
                    printf(", %d", pidsX[j]);
                }
                printf("\n");
            }
        }

        if (i == 1) {
            printf("Soy el super padre %d, mis hijos finales son: ", getpid());
            for (i = 0; i < y; i++) {
                printf("%d", pidsY[i]);
                if (i != y - 1) {
                    printf(", ");
                }
            }
            printf("\n");

            shmdt(pidsX);
            shmdt(pidsY);
            shmctl(shm_x, IPC_RMID, NULL);
            shmctl(shm_y, IPC_RMID, NULL);
        } else {
            if (i == x + 1) {
                for (i = 1; i <= y; i++) {
                    pid = fork();
                    if (pid == 0) {
                        pidsY[i - 1] = getpid();
                        signal(SIGALRM, despierta);
                        alarm(10);
                        pause();
                        break;
                    }
                }
                if (i == y + 1) {
                    for (i = 1; i <= y; i++) {
                        wait(NULL);
                    }
                }
            }
        }
    }
    return 0;
}