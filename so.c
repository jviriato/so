#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <math.h>
#include <time.h>


void gera_arvore_branch(int nivel, int altura, int root)    
{
    pid_t ArvEsq, ArvDir;

    if(nivel == 0){
        printf("------------- Branch -------------\n");
        printf("n = %d      PID=%d (root)\n", nivel, getpid());
    }

    nivel++;

    if (nivel > altura) 
        return;

    ArvEsq = fork();

    if (ArvEsq < 0){   
        fprintf(stderr, "fork falhou\n");
        exit(-1);
    }
    
    else if (ArvEsq == 0){  
        printf("n = %d      C[%d, %d]\n", nivel, getpid(), getppid());
        gera_arvore_branch(nivel, altura, root);
        printf("           T[%d, %d]\n", getpid(), getppid());
        exit(0);
    }
    
    else{     
        wait(&ArvEsq);
        ArvDir = fork();

    if (ArvDir < 0){    
        fprintf(stderr, "fork falhou\n");
        exit(-1);
    }
    else if (ArvDir == 0){   
        printf("n = %d      C[%d, %d]\n", nivel, getpid(), getppid());
        gera_arvore_branch(nivel, altura, root);
        printf("           T[%d, %d]\n", getpid(), getppid());
        exit(0);
    }
        wait(&ArvDir);
    }
    if(getpid() == root)
        printf("--- Fim criacao por Branch --\n");

    return;
}

void gera_arvore_livre(int nivel, int altura, int root)    
{
    pid_t ArvEsq, ArvDir;

    if(nivel == 0){
        printf("\n------------- Livre -------------\n");
        printf("n = %d      PID=%d (root)\n", nivel, getpid());
    }

    nivel++;

    if (nivel > altura) 
        return;

    ArvEsq = fork();

    if (ArvEsq < 0){   
        fprintf(stderr, "fork falhou\n");
        exit(-1);
    }
    
    else if (ArvEsq == 0){  
        printf("n = %d      C[%d, %d]\n", nivel, getpid(), getppid());
        gera_arvore_livre(nivel, altura, root);
        printf("           T[%d, %d]\n", getpid(), getppid());
        exit(0);
    }
    
    else{     
        ArvDir = fork();

    if (ArvDir < 0){    
        fprintf(stderr, "fork falhou\n");
        exit(-1);
    }
    else if (ArvDir == 0){   
        printf("n = %d      C[%d, %d]\n", nivel, getpid(), getppid());
        gera_arvore_livre(nivel, altura, root);
        printf("           T[%d, %d]\n", getpid(), getppid());
        exit(0);
    }
        wait(&ArvEsq);
        wait(&ArvDir);
    }
    if(getpid() == root)
        printf("--- Fim criacao por Livre --\n");

    return;
}


int main(int argc, char **argv)
{
    int altura = atoi(argv[1]);

    if (argc == 2 && altura > 1) {
        printf("A altura da árvore é de: %d\n", altura);
        double num_Processos = pow(2, altura+1) - 1;
        printf("Serão criados %.lf processos.\n\n", num_Processos);
        
        pid_t root = getpid();


        clock_t t0_branch = clock();
        gera_arvore_branch(0, altura, getpid());
        clock_t t1_branch = clock();
        double tempo_branch = (t1_branch-t0_branch)*1000.0 / CLOCKS_PER_SEC;
        
        clock_t t0_livre = clock();
        gera_arvore_livre(0, altura, getpid());
        clock_t t1_livre = clock();
        double tempo_livre = (t1_livre-t0_livre)*1000.0 / CLOCKS_PER_SEC;
 

        if(getpid() == root){
            printf ("Tempo execucao 'Branch': %g ms\n", tempo_branch);
            printf ("Tempo execucao 'Livre': %g ms\n", tempo_livre);

        }
    }
    else {
        printf("Entrada inválida\n");
    }
    printf("Fim PID = %d (root)\n", getpid());

    return 0;
}

