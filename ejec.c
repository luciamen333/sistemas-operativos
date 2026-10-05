#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

pid_t arb, a, b, x, y, z;

void puente(int sig) {

}

void tiempo(int sig) {
    char arb_cad[100];
    sprintf(arb_cad, "%d", arb);
    
    if (fork() == 0) {
        execlp("pstree", "pstree", "-c", arb_cad, NULL);
        exit(0);
    } else {
        wait(NULL);
        printf("mandando señal a arb: %d\n", arb);
        kill(arb, SIGUSR2);
        pause();
    }
}

void destroy_ejec(int sig) {
    printf("ejec => arb\n");
    kill(a, SIGUSR2);
}

void destroy_A(int sig) {
    printf("A => B\n");
    kill(b, SIGUSR2);
}

void muerte_hijos(int sig) {
    printf("B manda señales a hijos\n");
    kill(x, SIGUSR2);
    wait(NULL);
    
    kill(y, SIGUSR2);
    wait(NULL);
    
    kill(z, SIGUSR2);
    wait(NULL);
    
    exit(0);
}

int main(int argc, char *argv[]) {
    int t_espera;

    if (argc != 2) {
        printf("Usage: ./ejec tiempo\n");
        return 1;
    }

    t_espera = atoi(argv[1]);

    if (t_espera <= 0) {
        printf("Error: El tiempo debe ser mayor que 0\n");
        return 1;
    }

    arb = getpid();
    printf("Soy el proceso ejec: mi pid es %d\n", arb);

    a = fork();

    if (a != 0) {
        signal(SIGUSR2, destroy_ejec);
        pause();
        
        wait(NULL);
        printf("Soy ejec (%d) y muero\n", arb);
        exit(0);
    } else {
        a = getpid();
        printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n", a, arb);

        b = fork();

        if (b != 0) {
            signal(SIGUSR1, tiempo);
            signal(SIGUSR2, destroy_A);
            pause();
            
            wait(NULL);
            printf("Soy A (%d) y muero\n", a);
            exit(0);
        } else {
            b = getpid();
            printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n", b, a, arb);

            x = fork();
            if (x == 0) {
                printf("Soy el proceso X: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo %d\n", getpid(), b, a, arb);
                signal(SIGUSR2, puente);
                pause();
                printf("Soy X (%d) y muero\n", getpid());
                exit(0);
            }

            y = fork();
            if (y == 0) {
                printf("Soy el proceso Y: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo %d\n", getpid(), b, a, arb);
                signal(SIGUSR2, puente);
                pause();
                printf("Soy Y (%d) y muero\n", getpid());
                exit(0);
            }

            z = fork();
            if (z == 0) {
                printf("Soy el proceso Z: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo %d\n", getpid(), b, a, arb);
                signal(SIGALRM, puente);
                alarm(t_espera);
                pause();

                kill(a, SIGUSR1);
                signal(SIGUSR2, puente);
                pause();
                printf("Soy Z (%d) y muero\n", getpid());
                exit(0);
            }

            signal(SIGUSR2, muerte_hijos);
            pause();
        }
    }

    return 0;
}