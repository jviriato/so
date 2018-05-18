#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct{
    int    idThread;
    char*  pt_vetor;
} args;


char letra_aleatoria(){
    return 'a' + rand() % 26;
}

void preenche_vetor(char* vetor, int N)
{
    for (int i = 0; i < N; i++){
        vetor[i] = letra_aleatoria();
    }
}

void *imprimeVetor (void *arg){
  args *vargs = (args *) arg;

  printf("Sou a thread %d e estou apontando para a posição %c do vetor\n", vargs->idThread, (char)vargs->pt_vetor[vargs->idThread]);
  free(arg);
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

    srand (time(NULL));
    if( argc > 1 ){
        
        int N = atoi(argv[1]);
        char *vetor = (char*)malloc(sizeof(char) * N);
        preenche_vetor(vetor, N);
        printf("Vetor: %s\n", vetor);

        pthread_t tid[N];
        args *arg;

        for(int i = 0; i < N; i++){
            arg = malloc(sizeof(args));
            arg->idThread = i;
            arg->pt_vetor = vetor;
            if (pthread_create(&tid[i], NULL, imprimeVetor, (void*) arg)) {
                printf("erro ao criar thread\n"); 
                exit(-1);
            }
        }
        pthread_exit(NULL);
        free(vetor);
    }
    else
        printf("Envie N por linha de comando\n");
}