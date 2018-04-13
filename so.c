#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <math.h>

#define ROOT getpid()

int gera_arvore(int nivel, int altura)
{
    pid_t ArvEsq, ArvDir;
    nivel++;
    if (nivel <= altura){
        ArvEsq = fork();
        if (ArvEsq < 0){
            printf("Erro.\n");
        }
        else if (ArvEsq == 0){
			printf("n = %d        C[%d, %d]\n", nivel, getpid(), getppid());
            gera_arvore(nivel, altura);
        }
        else {
            wait(&ArvEsq);
            ArvDir = fork();
            if (ArvDir < 0){
                printf("Erro.\n");
            }

            if (ArvDir == 0){
    			printf("n = %d        C[%d, %d]\n", nivel, getpid(), getppid());
                gera_arvore(nivel, altura);
            }
            else {
				wait(&ArvDir);
				printf("             T[%d, %d]\n", getpid(), getppid());
			 	if (ROOT != getpid())				
					exit(1);
				else
					return 5;
            }
        }
    }
    else {
        printf("             T[%d, %d]\n", getpid(), getppid());
    }
    

}

int main(int argc, char **argv)
{
    if (argc == 2 && atoi(argv[1]) > 0) {
        int altura = atoi(argv[1]);
        printf("A altura da árvore é de: %d\n", altura);


        double num_Processos = pow(2, altura+1) - 1;
        printf("Serão criados %lf processos.\n\n", num_Processos);
        printf("n = 0        PID: %d (root)\n", getpid());
        gera_arvore(0, altura);
    }
    else {
        printf("Entrada inválida\n");
    }
    return 0;
}
