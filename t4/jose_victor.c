#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <math.h>

#define TRUE 1

sem_t mutex;                  
sem_t operators;            
sem_t clients;              
sem_t mutex_wait;
int queue = 0;
int num_linhas;
int num_atendentes;
int *fila_espera;


typedef struct{
	int tempo_intervalo;
	int atendidos;	              
	int intervalo;	             
	int atendidos_total;       	
	int disponivel;	        	
}atendente;
atendente **v_atendentes;


void callcenter();
void* operator(void *id_operator);
void* client(void *id_client);
int escolhe_operador();
int tem_pausa(int id);
int remove_queue();
int add_queue(int id_client);

int add_queue(int id_client)
{
    queue++;
	for(int i = 0; i < num_linhas; i++){
		if(fila_espera[i] == 0){
			fila_espera[i] = id_client;
			break;
		}
	}
}

int remove_queue()
{
	queue--;
	int cliente = fila_espera[0];
	for(int i = 0; i < num_linhas-1; i++){
		fila_espera[i] = fila_espera[i+1];
	}
	fila_espera[num_linhas-1] = 0;
	return cliente;
}

int escolhe_operador(){
	int i, menor_valor, prox_atendente;
	menor_valor = v_atendentes[0]->atendidos_total;
	prox_atendente = 1;
	for(i = 1; i < num_atendentes; i++){
		if(v_atendentes[i]->atendidos_total < menor_valor)
            if(v_atendentes[i]->intervalo == 0)
                if(v_atendentes[i]->disponivel == 1){
                    menor_valor = v_atendentes[i]->atendidos_total;
                    prox_atendente = i+1;
		        }
	}
	return prox_atendente;
}

int tem_pausa(int id)
{
    int limite = 2; // num maximo de clientes que podem ser atendidos
 	if(v_atendentes[id-1]->atendidos < limite){
		return 0;
	}
	for(int i = 0; i < num_atendentes; i++){
			if(v_atendentes[i]->intervalo == 1){
				return 0;
		}
	}
	return 1;   
}

void* operator(void *id_operator)
{
    int id_op = (intptr_t) id_operator;
    int cliente;
    while(TRUE){
        sem_wait(&mutex_wait);                     // para ver se o atendente pode ter um break
        if(tem_pausa(id_op))
            v_atendentes[id_op-1]->intervalo = 1; //operador em pausa
        sem_post(&mutex_wait);
        if(v_atendentes[id_op-1]->intervalo){   
            printf("Atendente %d atendeu %d clientes. Descansando...\n", id_op, v_atendentes[id_op-1]->atendidos);
            v_atendentes[id_op-1]->disponivel = 0;
            sleep(v_atendentes[id_op-1]->tempo_intervalo);
            printf("Atendente %d voltou do descanso!\n", id_op);
            v_atendentes[id_op-1]->atendidos = 0;	//reset
            v_atendentes[id_op-1]->intervalo = 0;	//
            v_atendentes[id_op-1]->disponivel = 1;	//
        }
        int t = (rand()%7) + 3;	//calcula tempo de espera do atendimento
		if(escolhe_operador() == id_op) {	//verifica se é o que menos atendeu no total
			sem_wait(&clients);     //espera algum cliente realizar ligação
			sem_wait(&mutex);	//inicio de região critica
			v_atendentes[id_op-1]->disponivel = 0;
			cliente = remove_queue();
			printf("Atendente %d está falando com cliente %d! Há %d chamadas em espera \n", id_op, cliente, queue);
			sem_post(&mutex);	
			sem_post(&operators);	//atendente fica disponivel para proxima ligação
            v_atendentes[id_op-1]->atendidos++;
            v_atendentes[id_op-1]->atendidos_total++;
			sleep(t);		//tempo do atendimento
			v_atendentes[id_op-1]->disponivel = 1;
			printf("Atendente %d terminou de atender o cliente %d!\n", id_op, cliente);
		}
    }
    pthread_exit(NULL);
}

void* client(void *id_client)
{
	int id_c = (intptr_t) id_client;
	printf("Cliente %d está ligando! Havia %d chamadas em espera\n", id_c, queue);
	sem_wait(&mutex);	//cliente entra na linha de espera
	add_queue(id_c);
	sem_post(&mutex);
	sem_post(&clients);	     
	sem_wait(&operators);	
	printf("Cliente %d está sendo atendido! Há %d chamadas em espera\n", id_c, queue);
	pthread_exit(NULL);

}

void callcenter_init()
{
    sem_init(&clients, TRUE, 0);
    sem_init(&operators, TRUE, 0);
    sem_init(&mutex, TRUE, 1);
    sem_init(&mutex_wait, TRUE, 1);
    fila_espera = malloc(sizeof(int) * num_linhas);
    for(int i = 0; i < num_linhas; i++) 
        fila_espera[i] = 0;
    v_atendentes = malloc(sizeof(atendente*) * num_atendentes);
}

void callcenter()
{
    printf("----------------------------------------------------------------------------------\n"
    "Call Center do José Victor (número de linhas de espera: %d, número de atendentes: %d)\n"
    "----------------------------------------------------------------------------------\n", num_linhas, num_atendentes);

    pthread_t *o, c;
    callcenter_init();
    o = malloc(sizeof(pthread_t) * num_atendentes);

    int id_operator = 0;
    for(int i = 0; i < num_atendentes; i++){
        v_atendentes[i] = malloc(sizeof(atendente));
        id_operator = i+1;
        v_atendentes[i]->atendidos = 0;
        v_atendentes[i]->atendidos_total = 0;
        v_atendentes[i]->intervalo = 0;
        v_atendentes[i]->disponivel = 1;
        v_atendentes[i]->tempo_intervalo = (rand()%5) + 7;
        pthread_create(&o[i], NULL, operator, (void*)(intptr_t)id_operator);
    }

    int id_client = 1;
    while(TRUE){
        if(queue < num_linhas){		
            pthread_create(&c, NULL, client, (void*)(intptr_t)id_client);	
        }
        else{	
            printf("Cliente %d não consegue realizar chamada. Todas as linhas ocupadas\n", id_client);
        }
        id_client++;
        int t = (rand()%2) + 2;
        sleep(t);	
    }
	pthread_join(c, NULL);
	for(int i = 0; i < num_atendentes; i++){
		pthread_join(o[i], NULL);
	}
}


int main(int argc, char const *argv[])
{
    num_linhas = atoi(argv[2]);
    num_atendentes = atoi(argv[1]);
    
    if (argc == 3 && num_linhas > 0 && num_atendentes > 0) {
        srand(time(NULL));
        callcenter();
    }
    else {
        printf("Insira dois números inteiros positivos não nulos.\n");
    }
    return 0;
}
