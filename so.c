#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <math.h>
#include <time.h>


#define ROOT getpid()

void gera_arvore_branch(int nivel, int altura, int pid_root);
void gera_arvore_livre(int nivel, int altura);

int main(int argc, char **argv)
{
    if (argc == 2 && atoi(argv[1]) > 0) {
        int altura = atoi(argv[1]);
        printf("A altura da árvore é de: %d\n", altura);

        double num_Processos = pow(2, altura+1) - 1;
        printf("Serão criados %lf processos.\n\n", num_Processos);
        

        pid_t id = getpid();

        clock_t inicio_tempo_Branch = clock();
        gera_arvore_branch(0, altura, ROOT);

        // double duracao = (fim_tempo_Branch - inicio_tempo_Branch)*1000.0 / CLOCKS_PER_SEC;
        // printf("Tempo de execucao da Arvore: %f s\n", duracao);

        if(getpid() == id){
            clock_t fim_tempo_Branch = clock();
            double duracao = (fim_tempo_Branch - inicio_tempo_Branch)*1000.0 / CLOCKS_PER_SEC;
		    printf("Tempo de execucao da Arvore: %f s\n", duracao);
        }
        // gera_arvore_livre(0, altura);


    }
    else {
        printf("Entrada inválida\n");
    }
    return 0;
}

void gera_arvore_branch(int nivel, int altura, int pid_root)
{
    pid_t ArvEsq, ArvDir;

    if (nivel == 0){
        printf("--------------- Branch ---------------\n");
        printf("n = 0        PID: %d (root)\n", getpid());
    }
    nivel++;
    if (nivel <= altura){
        ArvEsq = fork();
        if (ArvEsq < 0){
			fprintf(stderr, "fork falhou\n");
        }
        else if (ArvEsq == 0){
			printf("n = %d        C[%d, %d]\n", nivel, getpid(), getppid());
            gera_arvore_branch(nivel, altura, pid_root);
        }
        else {
            wait(&ArvEsq);
            ArvDir = fork();
            if (ArvDir < 0){
                printf("Erro.\n");
            }

            if (ArvDir == 0){
    			printf("n = %d        C[%d, %d]\n", nivel, getpid(), getppid());
                gera_arvore_branch(nivel, altura, pid_root);
            }
            else {
				wait(&ArvDir);
				printf("             T[%d, %d]\n", getpid(), getppid());
			 	if (ROOT != getpid())				
					exit(1); 
                else if (getpid() == pid_root)
                    printf("----- Fim criacao do Branch -----\n");
                    return;
            }
        }
    }
    else {
        printf("             T[%d, %d]\n", getpid(), getppid());
        exit(0);
    }
}


// void gera_arvore_livre(int nivel, int altura)
// {
//     pid_t ArvEsq, ArvDir;

//     if (nivel == 0){
//         printf("--------------- Livre ---------------\n");
//         printf("n = 0        PID: %d (root)\n", getpid());
//     }

//     nivel++;
//     if (nivel <= altura){
//         ArvEsq = fork();
//         if (ArvEsq < 0){
// 			fprintf(stderr, "fork falhou\n");
//         }
//         else if (ArvEsq == 0){
// 			printf("n = %d        C[%d, %d]\n", nivel, getpid(), getppid());
//             gera_arvore_branch(nivel, altura);
//         }
//         else {
//             ArvDir = fork();
//             if (ArvDir < 0){
//                 printf("Erro.\n");
//             }

//             if (ArvDir == 0){
//     			printf("n = %d        C[%d, %d]\n", nivel, getpid(), getppid());
//                 gera_arvore_branch(nivel, altura);
//             }
//             else {
//                 wait(&ArvEsq);
// 				wait(&ArvDir);
// 				printf("             T[%d, %d]\n", getpid(), getppid());
// 			 	if (ROOT != getpid())				
// 					exit(1);
// 				else
// 					return;
//             }
//         }
//     }
//     else {
//         printf("             T[%d, %d]\n", getpid(), getppid());
//     }
    

// }
