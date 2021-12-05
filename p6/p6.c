// gcc -o p6 p6.c -lpthread

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#include <stdlib.h>
#include <time.h>

#include <unistd.h>

/*
  una tribu cena en comunidad una olla de M misioneros
  cundo un miembro de la tribu quiere comer, el mismo se sirve un misionero
  van de uno en uno
  si la olla está vacia, el que cena despierta al cocinero y se espera a que llene la olla
  
  olla: entero que indica el numero de misioneros en la olla, inicializado a M
  mutex: semaforo para proteger exclusion mutua sobre la olla, inicializado a 1
  espera: semaforo utilizado para que el que va a cenar se detenga hasta que el cocinero
    llene la olla, inicializado a 0
  coci: semaforo inicializado a 0 y usado para que el cocinero no haga nada cuando la olla
    esta vacia
*/

//se asume M como tamaño de la olla
#define M 10

//canibales, consumidores
#define C 3

#define true 1
#define false 0


int olla = M;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t espera;
sem_t coci;

void *cocinar(void *arg) {
  int i, id;
  
  id = *(int *) arg;
  unsigned int seed = time(NULL) ^ id;
  
  while (true) {
    sem_wait(&coci); //cocinero durmiendo
    
    printf("Cocinero llenando olla...\n");
    //rellena olla
    for(i=0; i<M; ++i) {
      sleep(rand_r(&seed) % 3);
      olla++;
      printf("\t%d/%d misioneros...\n", olla, M);
    }
    
    sem_post(&espera); //avisa al canibal esperando
  }
}

void *comer(void *arg) {
  int id;
  
  id = *(int *) arg;
  unsigned int seed = time(NULL) ^ id;
  
  while (true) {
    //canibal pretende servirse
    pthread_mutex_lock(&mutex);
      if (olla == 0 ) {
        printf("Olla vacia, canibal %d avisando cocinero\n", id);
        sem_post(&coci); //despierta al cocinero
        sem_wait(&espera); //espera a que el cocinero acabe
      }
      
      olla--; //se sirve
      printf("Canibal %d comiendo, quedan %d/%d misioneros\n", id, olla, M);
    pthread_mutex_unlock(&mutex);
    
    sleep(rand_r(&seed) % 10); //canibal comiendo
  }
  
  pthread_exit(NULL);
}

int main() {
  int i;
  
  sem_init(&espera, 0, 0);
  sem_init(&coci, 0, 0);
  
  int ids[C+1];
  for(i=0; i<C; ++i)
    ids[i] = i;
  ids[C] = -1;
  
  pthread_t cocinero;
  pthread_t canibales[C];

  for(i=0; i<C; ++i)
    pthread_create(&canibales[i], NULL, comer, &ids[i]);
  pthread_create(&cocinero, NULL, cocinar, &ids[C]);

  for(i=0; i<C; ++i)
    pthread_join(canibales[i], NULL);
  pthread_join(cocinero, NULL);

  return 0;
}































