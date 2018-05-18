#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int indice = 0;
char* vetor;

typedef struct{
    char    idThread;
    int V;
} args;

void preenche_vetor(char* vetor, int N)
{
    for (int i = 0; i < N; i++){
        vetor[i] = '-';
    }
}

void *imprimeVetor (void *arg){
    args *vargs = (args *) arg;

    for(int i = 0; i < vargs->V; i++){
        vetor[indice] = vargs->idThread;
        usleep(5000);
        indice++;
    }

    free(arg);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

    srand (time(NULL));
    if( argc > 2 ){
        
        int N = atoi(argv[1]);
        int V = atoi(argv[2]);
        vetor = (char*)malloc(sizeof(char) * N * V);
        preenche_vetor(vetor, N*V);
        printf("Vetor: %s\n", vetor);

        pthread_t tid[N];
        args *arg;

        for(int i = 0; i < N; i++){
            arg = malloc(sizeof(args));
            arg->idThread = i + 'a';
            arg->V = V;
            if (pthread_create(&tid[i], NULL, imprimeVetor, (void*) arg)) {
                printf("erro ao criar thread\n"); 
                exit(-1);
            }
            pthread_join(tid[i], NULL);

        }
        printf("\nVetor: %s\n", vetor);

        pthread_exit(NULL);
        free(vetor);
    }
    else
        printf("Envie N e V por linha de comando\n");
        
}
