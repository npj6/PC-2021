// gcc -o p5 p5.c -lpthread

#include <stdio.h>
#include <pthread.h>

#include <stdlib.h>
#include <time.h>

#include <unistd.h>

#define NUM_LECTORES 10
#define NUM_LECTURAS 10

#define NUM_ESCRITORES 5

#define true 1
#define false 0

int readers = 0;
pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;

int recurso = -1;
pthread_mutex_t writer = PTHREAD_MUTEX_INITIALIZER;

void *escribir(void *arg) {
  int i, id;
  
  id = *(int *) arg;
  
  unsigned int seed = time(NULL) ^ id;
  int waiting_time = rand_r(&seed) % 10;
  sleep(waiting_time);
  
  pthread_mutex_lock(&writer);
  
  recurso = id;
  printf("El escritor %d ha actualizado el recurso\n", id);
  
  pthread_mutex_unlock(&writer);
  
  pthread_exit(NULL);
}

void *leer(void *arg) {
  int i, id;
  
  id = *(int *) arg;
  
  unsigned int seed = time(NULL) ^ id;
  int waiting_time;
  
  for(i=0; i<NUM_LECTURAS; ++i) {
    waiting_time = rand_r(&seed) % 3;
    sleep(waiting_time);
  
    // se intenta incrementar el numero de readers
    pthread_mutex_lock(&mx);
      if (readers == 0) //si no hay lectores aun, espera a que no hayan escritores
        pthread_mutex_lock(&writer); //el primer lector bloquea la escritura
      readers++;
    pthread_mutex_unlock(&mx);
    
    printf("El lector %d ha leido un valor de %d\n", id, recurso);
    
    //se intenta decrementar el numero de readers
    pthread_mutex_lock(&mx);
      readers--;
      
      if (readers == 0)
        pthread_mutex_unlock(&writer); //el ultimo lector libera la escritura

    pthread_mutex_unlock(&mx);
  }
  
  pthread_exit(NULL);
}

int main() {
  int i, highest;
  
  highest = (NUM_LECTORES < NUM_ESCRITORES ? NUM_ESCRITORES : NUM_LECTORES);
  int ids[highest];
  for(i=0; i<highest; ++i)
    ids[i] = i;
  
  pthread_t lectores[NUM_LECTORES];
  pthread_t escritores[NUM_ESCRITORES];

  for(i=0; i<NUM_LECTORES; ++i)
    pthread_create(&lectores[i], NULL, leer, &ids[i]);
  for(i=0; i<NUM_ESCRITORES; ++i)
    pthread_create(&escritores[i], NULL, escribir, &ids[i]);

  for(i=0; i<NUM_LECTORES; ++i)
    pthread_join(lectores[i], NULL);
  for(i=0; i<NUM_ESCRITORES; ++i)
    pthread_join(escritores[i], NULL);

  return 0;
}































