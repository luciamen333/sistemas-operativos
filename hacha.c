#include <stdio.h> 
#include <sys/wait.h>
#include <sys/stat.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h>

void partir(char *nombre, int tamanyo);

int main(int argc, char *argv[]) {
    if (argc != 3) {  
        printf("Error. Uso: %s fichero tam_trozo\n", argv[0]);
    } else {
        partir(argv[1], atoi(argv[2]));
    }
    return 0;
}

void partir(char nombre[], int tamanyo) {
    int i;  
    int tubo[2];
    char nombreFichero[50];
    int fEntrada, fSalida; 
    struct stat propFichero;
    int numTrozos, numLeidos;
    char *trozoLeido;       
        
    trozoLeido = (char *) malloc(sizeof(char) * tamanyo);
    fEntrada = open(nombre, O_RDONLY);

    if (fEntrada < 0) {
        printf("Error. No existe el fichero\n");
    } else {
        stat(nombre, &propFichero);
        numTrozos = propFichero.st_size / tamanyo;

        if (propFichero.st_size % tamanyo != 0) {
            numTrozos++;
        }

        for (i = 0; i < numTrozos; i++) {
            pipe(tubo);
            if (fork() != 0) {
                numLeidos = read(fEntrada, trozoLeido, tamanyo);
                write(tubo[1], trozoLeido, numLeidos);
            } else { 
                if (i < 10) {
                    sprintf(nombreFichero, "%s.h0%d", nombre, i);
                } else {
                    sprintf(nombreFichero, "%s.h%d", nombre, i);
                }

                fSalida = creat(nombreFichero, 0666);   
                numLeidos = read(tubo[0], trozoLeido, tamanyo);
                write(fSalida, trozoLeido, numLeidos);
                
                close(fSalida);
                free(trozoLeido);
                break;
            }
        }

        if (i == numTrozos) {
            free(trozoLeido);
            for (int j = 0; j < numTrozos; j++) {
                wait(NULL);
            }
        }
    }
}