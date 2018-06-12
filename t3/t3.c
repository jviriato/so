#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <semaphore.h>


#define TRUE 1

int queue = 0;
int queue_max;
int id_client = 0;
sem_t mutex;
sem_t operators;
sem_t clients;

void* operator(void *arg);
void* client(void *arg);
void atende_cliente();
void client_called();
void cliente_atendido();
void giveup_call();


void* operator(void *arg)
{
    while(TRUE) {
        sem_wait(&clients);     /* vai tomar café se o número de clientes for 0 */
        sem_wait(&mutex);       /* obtém acesso a queue */
        queue = queue - 1;      /* descresce de um o contador de clientes à espera */
        sem_post(&operators);   /* um atendente está agora pronto para atender ligações */
        sem_post(&mutex);       /* libera queue */
        atende_cliente();       /* atende cliente (fora da região crítica) */
    }
    pthread_exit(NULL);
}

void* client(void *arg)
{

    sem_wait(&mutex);           /* entra na região crítica */
    if(queue < queue_max) {     /* se não houver linhas disponíveis, desligue */
        client_called();
        queue = queue + 1;      /* incrementa o contador de clientes à espera */
        sem_post(&clients);     /* acorda o operador se necessário */
        sem_post(&mutex);       /* libera o acesso a fila */
        sem_wait(&operators);   /* dorme se operadores livres for 0 */
        cliente_atendido();     /* ligação realizada */
    } 
    else {
        sem_post(&mutex);       /* fila cheia, desiste */
        giveup_call();
    }
    pthread_exit(NULL);

}

void atende_cliente() {
printf("Atendente esta falando com algum cliente! Ha %d chamadas em espera\n", queue);
sleep(rand () % 3 + 1);
}

void client_called() {
printf("Cliente %d está ligando! Havia %d chamadas em espera\n", id_client, queue);
}
void cliente_atendido() {
printf("Cliente %d está sendo atendido!\n", id_client);
}

void giveup_call() {
printf("Cliente %d não consegue realizar a chamada. Todas as linhas ocupadas\n", id_client);
}

void callcenter(int num_linhas)
{
    pthread_t o, c;
    queue_max = num_linhas;

    sem_init(&clients, TRUE, 0);
    sem_init(&operators, TRUE, 0);
    sem_init(&mutex, TRUE, 1);


    // cria operador de telemarketing
    pthread_create(&o, NULL, (void *) operator, NULL);
    // cria clientes
    while(TRUE) {
        id_client = id_client + 1;
        pthread_create(&c, NULL, (void *) client, NULL);
        sleep(1);
    }

}

int main(int argc, char const *argv[])
{
    if (argc == 2 && argv[1] > 0){
        int num_linhas = atoi(argv[1]);
        srand(time(NULL));
        callcenter(num_linhas);
    }
    else {
        printf("Insira um número inteiro positivo não nulo.\n");
    }
    return 0;
}

