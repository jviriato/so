#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

typedef struct{
    int init;
    int end;
} args;

typedef struct{
    int init;
    int end;
    int ratio;
} args_esparsa;

typedef struct{
    int msec;
    int qtd_defectivo;
    int qtd_abundante;
    int qtd_perfeito;
} args_return;

int soma_de_divisores(int n)
{
    int sum = 0;
    for (int i = 1; i < n; i++){
        if (n % i == 0){
            sum += i;
        }
    }
    return sum;
}

int sequencial(int worksize)
{
    clock_t start = clock(), diff;
    int v[worksize];
    int qtd_defectivo = 0;
    int qtd_abundante = 0;
    int qtd_perfeito  = 0;

    for(int i = 0; i < worksize; i++){
        v[i] = soma_de_divisores(i);
        if(v[i] > i)
            qtd_abundante++;
        else if(v[i] < i){
            // printf("defectivo: %d\n", i);
            qtd_defectivo++;
        }
        else if (v[i] == i)
            qtd_perfeito++;
    }
    printf("SEQUENCIAL:\nqtd. Defectivo: %d\nqtd. Abundante: %d\nqtd. Perfeito: %d\n", qtd_defectivo, qtd_abundante, qtd_perfeito);
    diff = clock() - start;
    return diff * 1000 / CLOCKS_PER_SEC;
}

void *threads_com_chunk(void* arg)
{
    clock_t start = clock(), diff;
    args *vargs = (args *) arg;
    args_return *resultado = malloc(sizeof(args_return));    
    int v[vargs->end - vargs->init+1];
    int qtd_defectivo = 0;
    int qtd_abundante = 0;
    int qtd_perfeito  = 0;
    int j = 0;
    for (int i = vargs->init; i < vargs->end+1; i++){
        v[j] = soma_de_divisores(i);
        if(v[j] > i){
            qtd_abundante++;
        }
        else if(v[j] < i){
            qtd_defectivo++;
        }

        else if (v[j] == i){
            qtd_perfeito++;
        }
        j++;
    }
    resultado->qtd_abundante += qtd_abundante;
    resultado->qtd_defectivo += qtd_defectivo;
    resultado->qtd_perfeito  += qtd_perfeito;
    diff = clock() - start;
    resultado->msec = diff * 1000 / CLOCKS_PER_SEC;

    pthread_exit(resultado);
}

void *threads_esparsas(void* arg)
{
    clock_t start = clock(), diff;
    args_esparsa *vargs = (args_esparsa *) arg;
    args_return *resultado = malloc(sizeof(args_return));    
    int v[vargs->end - vargs->init+1];
    int qtd_defectivo = 0;
    int qtd_abundante = 0;
    int qtd_perfeito  = 0;
    int j = 0;

    for (int i = vargs->init; i < vargs->end; i+=vargs->ratio){
        
        v[j] = soma_de_divisores(i);
        if(v[j] > i){
            qtd_abundante++;
        }
        else if(v[j] < i){
            qtd_defectivo++;
        }

        else if (v[j] == i){
            qtd_perfeito++;
        }
        j++;
    }
    resultado->qtd_abundante += qtd_abundante;
    resultado->qtd_defectivo += qtd_defectivo;
    resultado->qtd_perfeito  += qtd_perfeito;
    diff = clock() - start;
    resultado->msec = diff * 1000 / CLOCKS_PER_SEC;

    pthread_exit(resultado);
}


void divide_trabalho(args* arg, int i, int* fim_do_anterior, int ratio, int* divisao, int threads)
{
    if(i == 0){
        arg->init = 0;
        arg->end  = arg->init + ratio;
        *fim_do_anterior = arg->end;
        (*divisao)--;
    }else if(*divisao > 0){
        arg->init = (*fim_do_anterior) + 1;
        arg->end  = arg->init + ratio;
        *fim_do_anterior = arg->end;
        (*divisao)--;
    }else{
        if(i == threads - 1){
            arg->init = (*fim_do_anterior) + 1; 
            arg->end = arg->init + ratio - 1;
            *fim_do_anterior = arg->end;
        }
        else{
            arg->init = (*fim_do_anterior) + 1; 
            arg->end = arg->init + ratio - 1;
            *fim_do_anterior = arg->end;
        }
    }
    // printf("init: %d end: %d\n", arg->init, arg->end);
    // printf("divisao: %d\n", *divisao);
}




int main(int argc, char *argv[])
{
    if(argc == 3){
        int worksize = atoi(argv[1]);
        int threads  = atoi(argv[2]);

        int msec = sequencial(worksize);
        printf("Time taken: %d seconds %d milliseconds\n\n", msec/1000, msec%1000);

        
        pthread_t tid[threads];
        args *arg;
        args_esparsa *arg_e;
        int divisao = worksize % threads;        
        int ratio   = worksize / threads;
        int soma_defectivo = 0;
        int soma_abundante = 0;
        int soma_perfeito = 0;

        if (divisao == 0){
            for(int i = 0; i < threads; i++){
                arg = malloc(sizeof(args));
                int init = ratio * i;
                arg->init = init;
                arg->end  = init + ratio - 1;            
                if (pthread_create(&tid[i], NULL, threads_com_chunk, (void*) arg)) {
                    printf("erro ao criar thread\n"); 
                    exit(-1);
                }
            }

        }else{
            int fim_do_anterior = ratio;
            for(int i = 0; i < threads; i++){
                arg = malloc(sizeof(args));
                divide_trabalho(arg, i, &fim_do_anterior, ratio, &divisao, threads);
                if (pthread_create(&tid[i], NULL, threads_com_chunk, (void*) arg)) {
                    printf("erro ao criar thread\n"); 
                    exit(-1);
                }
            }
        }
        msec = 0;
        for(int i = 0; i < threads; i++){
            args_return* soma;
            pthread_join(tid[i], (void**)&soma);
            soma_defectivo += soma->qtd_defectivo;
            soma_abundante += soma->qtd_abundante;
            soma_perfeito  += soma->qtd_perfeito;
            msec = soma->msec;
        }
        printf("C/ CHUNKS:\nqtd. Defectivo: %d\nqtd. Abundante: %d\nqtd. Perfeito: %d\n", soma_defectivo, soma_abundante, soma_perfeito);
        printf("Time taken: %d seconds %d milliseconds\n\n", msec/1000, msec%1000);

        for(int i = 0; i < threads; i++){
            arg_e = malloc(sizeof(args_esparsa));
            arg_e->init = i;
            arg_e->end  = worksize; 
            arg_e->ratio = threads;           
            if (pthread_create(&tid[i], NULL, threads_esparsas, (void*) arg_e)) {
                printf("erro ao criar thread\n"); 
                exit(-1);
            }
        }
        msec = 0;
        soma_abundante = 0;
        soma_defectivo = 0;
        soma_perfeito  = 0;
        for(int i = 0; i < threads; i++){
            args_return* soma;
            pthread_join(tid[i], (void**)&soma);
            soma_defectivo += soma->qtd_defectivo;
            soma_abundante += soma->qtd_abundante;
            soma_perfeito  += soma->qtd_perfeito;
            msec = soma->msec;
        }
        printf("ESPARSAS:\nqtd. Defectivo: %d\nqtd. Abundante: %d\nqtd. Perfeito: %d\n", soma_defectivo, soma_abundante, soma_perfeito);
        printf("Time taken: %d seconds %d milliseconds\n\n", msec/1000, msec%1000);



    pthread_exit(NULL);
    }
    else{
        printf("Insira qtd. de threads e worksize.\n");
    }
}