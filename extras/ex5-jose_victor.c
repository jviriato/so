#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct{
    int    idThread;
    int*   pt_vetor;
    int    tamanho_vetor;
} args;

typedef struct{
    int    idThread;
    int    total;
} args_soma;

void preenche_vetor(int* vetor, int N)
{
    for (int i = 0; i < N; i++){
        vetor[i] = rand() % 99;
    }
}

void *somaVetor (void *arg){
    args *vargs = (args *) arg;
    args_soma *soma = malloc(sizeof(args_soma));
    for(int i = 0; i < vargs->tamanho_vetor; i++){
        soma->idThread = vargs->idThread;
        soma->total += vargs->pt_vetor[i];
    }
    pthread_exit(soma);
}

int main(int argc, char *argv[])
{

    srand (time(NULL));
    if( argc > 2 ){
        
        int N = atoi(argv[1]);
        int V = atoi(argv[2]);

        int *vetor = (int*)malloc(sizeof(int) * V);
        preenche_vetor(vetor, V);
        for(int i = 0; i < V; i++){
            printf("%d ", vetor[i]);
        }
        printf("\n");
        pthread_t tid[N];
        args *arg;

        for(int i = 0; i < N; i++){
            arg = malloc(sizeof(args));
            arg->idThread = i;
            arg->pt_vetor = vetor;
            arg->tamanho_vetor = V;
            if (pthread_create(&tid[i], NULL, somaVetor, (void*) arg)) {
                printf("erro ao criar thread\n"); 
                exit(-1);
            }
        }
        for(int i = 0; i < N; i++){
            args_soma* soma;
            pthread_join(tid[i], (void**)&soma);
            printf("Thread %d, achou %d como a soma do vetor\n", soma->idThread, soma->total);
        }
        pthread_exit(NULL);
        free(vetor);

    }
    else
        printf("Envie N e V por linha de comando\n");
}