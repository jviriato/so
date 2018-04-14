#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>

long wtime(){
   struct timeval t;
   gettimeofday(&t, NULL);
   return t.tv_sec*1000000 + t.tv_usec;
}

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


        long t0_branch = wtime();
        gera_arvore_branch(0, altura, getpid());
        long t1_branch = wtime();
        double tempo_branch = (t1_branch - t0_branch);
        
        long t0_livre = wtime();
        gera_arvore_livre(0, altura, getpid());
        long t1_livre = wtime();
        double tempo_livre = (t1_livre - t0_branch);
 

        if(getpid() == root){
            printf ("Tempo execucao 'Branch': %lf seg\n", (tempo_branch / 1000000));
            printf ("Tempo execucao 'Livre': %lf seg\n", (tempo_livre / 1000000));

        }
    }
    else {
        printf("Entrada inválida\n");
    }
    printf("Fim PID = %d (root)\n", getpid());
    return 0;
}

