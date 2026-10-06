#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t pidEjec;
pid_t pidA;
pid_t pidB;
pid_t pidX;
pid_t pidY;
pid_t pidZ;

char procesoObjetivo;
int segundosEspera;

void leerArgumentos(int argc, char *argv[]);
void iniciarDestruccion(int s);

void crearProcesoA(void);
void ejecutarProcesoA(void);
void tareaProcesoA(int s);
void destruirProcesoA(int s);

void crearProcesoB(void);
void ejecutarProcesoB(void);
void tareaProcesoB(int s);
void destruirProcesoB(int s);

void crearProcesoX(void);
void ejecutarProcesoX(void);
void tareaProcesoX(int s);
void destruirProcesoX(int s);

void crearProcesoY(void);
void ejecutarProcesoY(void);
void tareaProcesoY(int s);
void destruirProcesoY(int s);

void crearProcesoZ(void);
void ejecutarProcesoZ(void);
void alarmaProcesoZ(int s);
void destruirProcesoZ(int s);

void leerArgumentos(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <proceso> <segundos>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    procesoObjetivo = argv[1][0];
    segundosEspera = atoi(argv[2]);

    if (procesoObjetivo != 'A' && procesoObjetivo != 'B' &&
        procesoObjetivo != 'X' && procesoObjetivo != 'Y') {
        fprintf(stderr, "Proceso objetivo invalido.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    leerArgumentos(argc, argv);

    pidEjec = getpid();
    printf("Soy el proceso ejec: mi pid es %d\n", pidEjec);

    signal(SIGUSR2, iniciarDestruccion);

    crearProcesoA();

    printf("Soy ejec (%d) y muero\n", pidEjec);
    return 0;
}

void iniciarDestruccion(int s) {
    (void)s;
    kill(pidA, SIGUSR2);
}

void crearProcesoA(void) {
    switch (pidA = fork()) {
        case 0:
            ejecutarProcesoA();
            exit(0);
        default:
            wait(NULL);
    }
}

void ejecutarProcesoA(void) {
    pidA = getpid();
    printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n", pidA, pidEjec);

    signal(SIGUSR1, tareaProcesoA);
    signal(SIGUSR2, destruirProcesoA);
    crearProcesoB();
}

void tareaProcesoA(int s) {
    pid_t pid;
    (void)s;

    printf("Soy el proceso A con %d, he recibido la senyal.\n", pidA);
    pid = fork();
    if (pid == 0) {
        execlp("pstree", "pstree", (char *)NULL);
        exit(EXIT_FAILURE);
    }

    wait(NULL);
    kill(pidEjec, SIGUSR2);
}

void destruirProcesoA(int s) {
    (void)s;

    kill(pidB, SIGUSR2);
    wait(NULL);

    printf("Soy A (%d) y muero\n", pidA);
    exit(0);
}

void crearProcesoB(void) {
    switch (pidB = fork()) {
        case 0:
            ejecutarProcesoB();
            exit(0);
        default:
            wait(NULL);
    }
}

void ejecutarProcesoB(void) {
    pidB = getpid();
    printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n",
           pidB, pidA, pidEjec);

    signal(SIGUSR1, tareaProcesoB);
    signal(SIGUSR2, destruirProcesoB);

    crearProcesoX();
    crearProcesoY();
    crearProcesoZ();

    while (1) {
        pause();
    }
}

void tareaProcesoB(int s) {
    pid_t pid;
    (void)s;

    printf("Soy el proceso B con %d, he recibido la senyal.\n", pidB);
    pid = fork();

    if (pid == 0) {
        execlp("pstree", "pstree", (char *)NULL);
        exit(EXIT_FAILURE);
    }

    wait(NULL);
    kill(pidEjec, SIGUSR2);
}

void destruirProcesoB(int s) {
    (void)s;

    kill(pidZ, SIGUSR2);
    wait(NULL);

    kill(pidY, SIGUSR2);
    wait(NULL);

    kill(pidX, SIGUSR2);
    wait(NULL);

    printf("Soy B (%d) y muero\n", pidB);
    exit(0);
}

void crearProcesoX(void) {
    switch (pidX = fork()) {
        case 0:
            ejecutarProcesoX();
            exit(0);
    }
}

void ejecutarProcesoX(void) {
    pidX = getpid();
    printf("Soy el proceso X: mi pid es %d. Mi padre es %d. Mi abuelo es %d...\n",
           pidX, pidB, pidA);

    signal(SIGUSR1, tareaProcesoX);
    signal(SIGUSR2, destruirProcesoX);

    while (1) {
        pause();
    }
}

void tareaProcesoX(int s) {
    pid_t pid;
    (void)s;

    printf("Soy el proceso X con %d, he recibido la senyal.\n", pidX);
    pid = fork();

    if (pid == 0) {
        execlp("ls", "ls", (char *)NULL);
        exit(EXIT_FAILURE);
    }

    wait(NULL);
    kill(pidEjec, SIGUSR2);
}

void destruirProcesoX(int s) {
    (void)s;

    printf("Soy X (%d) y muero\n", pidX);
    exit(0);
}

void crearProcesoY(void) {
    switch (pidY = fork()) {
        case 0:
            ejecutarProcesoY();
            exit(0);
    }
}

void ejecutarProcesoY(void) {
    pidY = getpid();
    printf("Soy el proceso Y: mi pid es %d. Mi padre es %d. Mi abuelo es %d...\n",
           pidY, pidB, pidA);

    signal(SIGUSR1, tareaProcesoY);
    signal(SIGUSR2, destruirProcesoY);

    while (1) {
        pause();
    }
}

void tareaProcesoY(int s) {
    pid_t pid;
    (void)s;

    printf("Soy el proceso Y con %d, he recibido la senyal.\n", pidY);
    pid = fork();

    if (pid == 0) {
        execlp("ls", "ls", (char *)NULL);
        exit(EXIT_FAILURE);
    }

    wait(NULL);
    kill(pidEjec, SIGUSR2);
}

void destruirProcesoY(int s) {
    (void)s;

    printf("Soy Y (%d) y muero\n", pidY);
    exit(0);
}

void crearProcesoZ(void) {
    switch (pidZ = fork()) {
        case 0:
            ejecutarProcesoZ();
            exit(0);
    }
}

void ejecutarProcesoZ(void) {
    pidZ = getpid();
    printf("Soy el proceso Z: mi pid es %d. Mi padre es %d. Mi abuelo es %d...\n",
           pidZ, pidB, pidA);

    signal(SIGALRM, alarmaProcesoZ);
    signal(SIGUSR2, destruirProcesoZ);

    alarm(segundosEspera);

    while (1) {
        pause();
    }
}

void alarmaProcesoZ(int s) {
    (void)s;

    switch (procesoObjetivo) {
        case 'A':
            kill(pidA, SIGUSR1);
            break;
        case 'B':
            kill(pidB, SIGUSR1);
            break;
        case 'X':
            kill(pidX, SIGUSR1);
            break;
        case 'Y':
            kill(pidY, SIGUSR1);
            break;
    }
}

void destruirProcesoZ(int s) {
    (void)s;

    printf("Soy Z (%d) y muero\n", pidZ);
    exit(0);
}