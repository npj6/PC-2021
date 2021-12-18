/*gcc -o barberia barberia.c -lpthread*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

//malloc, free, rand_r, time
#include <stdlib.h>
#include <time.h>

//sleep
#include <unistd.h>

//planteamiento raro??
//si el primer barbero empieza antes que el segundo
//pero el segundo acaba antes que el primero
//el segundo barbero liberará al cliente del primero
//y el primer babero seguirá cortando el pelo al segundo cliente

//idealmente cada barbero y silla son entidades individuales con mutex propio

#define TAM_TOTAL 20
#define TAM_SOFA 4
#define NUM_SILLAS 3
#define NUM_BARBEROS 3

/*sem_wait(&sem) sem_post(&sem)*/
//semaforos de hueco libre
sem_t max_capacidad, sofa, silla_barbero;
//semaforo de barberos libres
sem_t coord;
//semaforos sillas
sem_t cliente_listo;
sem_t dejar_silla_barbero;
//semaforo corte de pelo
sem_t terminado;
//semaforo pagos
sem_t pago, recibo;

/*
*/
#define true 1
#define false 0

// las esperas se realizan en ticks en lugar de segundos
// la constante TICK define el numero de segundos de un tick
#define TICK 3

typedef struct {
  unsigned int id, seed;
} ClienteInfo_T;

typedef ClienteInfo_T BarberoInfo_T;

void *cajeroFunc(void *arg) {
  unsigned int seed = *(unsigned int *) arg;
  int waitingTime;
  while(true) {
    printf("\033[36mCAJERO ESPERANDO PAGO\n\033[0m");
    sem_wait(&pago);

    printf("\033[36mCAJERO ESPERANDO BARBERO LIBRE\n\033[0m");
    sem_wait(&coord);

    printf("\033[36mCAJERO REALIZANDO TRANSACCION\n\033[0m");
    waitingTime = (rand_r(&seed) % 5) + 1;
    sleep(waitingTime * TICK);

    printf("\033[36mCAJERO HA TERMINADO TRANSACCION\n\033[0m");
    sem_post(&coord);
    sem_post(&recibo);
  }
}

void *barberoFunc(void *arg) {
  BarberoInfo_T barbero = *(BarberoInfo_T *) arg;
  free(arg);
  int waitingTime;

  while(true) {
    printf("\033[33mBARBERO %d ESPERANDO CLIENTE\n\033[0m", barbero.id);
    sem_wait(&cliente_listo);

    sem_wait(&coord);
    printf("\033[33mBARBERO %d CORTANDO PELO\n\033[0m", barbero.id);
    waitingTime = (rand_r(&barbero.seed) % 7) + 1;
    sleep(waitingTime * TICK);

    printf("\033[33mBARBERO %d HA ACABADO DE CORTAR PELO\n\033[0m", barbero.id);
    sem_post(&coord);
    sem_post(&terminado);
    
    printf("\033[33mBARBERO %d ESPERANDO A QUE LA SILLA SE LIBERE\n\033[0m", barbero.id);
    sem_wait(&dejar_silla_barbero);
    sem_post(&silla_barbero);
    printf("\033[33mBARBERO %d TIENE LA SILLA LIBRE\n\033[0m", barbero.id);
  }
}

void *clienteFunc (void *arg) {
  //crea una copia local y libera la memoria reservada
  ClienteInfo_T cliente = *(ClienteInfo_T *) arg;
  free(arg);
  int waitingTime;

  printf("\033[31mCLIENTE %d ENTRA EN ESCENA\n\033[0m", cliente.id);

  sem_wait(&max_capacidad);
  printf("\033[31mCLIENTE %d ENTRA A LA BABERIA\n\033[0m", cliente.id);
  waitingTime = (rand_r(&cliente.seed) % 2) + 1;
  sleep(waitingTime * TICK);

  sem_wait(&sofa);
  printf("\033[31mCLIENTE %d SE SIENTA EN EL SOFA\n\033[0m", cliente.id);
  waitingTime = (rand_r(&cliente.seed) % 2) + 1;
  sleep(waitingTime * TICK);
  
  sem_wait(&silla_barbero);
  printf("\033[31mCLIENTE %d SE LEVANTA DEL SOFA Y SE SIENTA EN UNA SILLA\n\033[0m", cliente.id);
  waitingTime = (rand_r(&cliente.seed) % 2) + 1;
  sleep(waitingTime * TICK);
  sem_post(&sofa);

  sem_post(&cliente_listo);
  printf("\033[31mCLIENTE %d ESTA RECIBIENDO UN CORTE DE PELO\n\033[0m", cliente.id);
  sem_wait(&terminado);
  printf("\033[31mCLIENTE %d HA RECIBIDO SU CORTE DE PELO\n\033[0m", cliente.id);

  printf("\033[31mCLIENTE %d SE LEVANTA DE LA SILLA\n\033[0m", cliente.id);
  waitingTime = (rand_r(&cliente.seed) % 2) + 1;
  sleep(waitingTime * TICK);
  sem_post(&dejar_silla_barbero);

  printf("\033[31mCLIENTE %d VA A PAGAR\n\033[0m", cliente.id);
  waitingTime = (rand_r(&cliente.seed) % 2) + 1;
  sleep(waitingTime * TICK);
  sem_post(&pago);

  printf("\033[31mCLIENTE %d ESPERANDO RECIBO\n\033[0m", cliente.id);
  sem_wait(&recibo);

  waitingTime = (rand_r(&cliente.seed) % 2) + 1;
  sleep(waitingTime * TICK);
  printf("\033[31mCLIENTE %d SE VA DE LA BARBERIA\n\033[0m", cliente.id);
  sem_post(&max_capacidad);

  pthread_exit(NULL);
}

int main () {
  unsigned int seed = time(NULL);
  unsigned int nextId = 0;
  unsigned int cajeroSeed;
  int waitingTime, i;
  pthread_t newThread; // ignoramos los id de los threads (no haremos join)

  //inicializamos semaforos
  sem_init(&max_capacidad, 0, TAM_TOTAL);
  sem_init(&sofa, 0, TAM_SOFA);
  sem_init(&silla_barbero, 0, NUM_SILLAS);
  sem_init(&coord, 0, NUM_BARBEROS);
  sem_init(&cliente_listo, 0, 0);
  sem_init(&dejar_silla_barbero, 0, 0);
  sem_init(&terminado, 0, 0);
  sem_init(&pago, 0, 0);
  sem_init(&recibo, 0, 0);

  cajeroSeed = rand_r(&seed);
  pthread_create(&newThread, NULL, cajeroFunc, &cajeroSeed);

  for(i=0; i<NUM_BARBEROS; ++i) {
    BarberoInfo_T* barbero = (BarberoInfo_T*) malloc(sizeof(BarberoInfo_T));
    barbero->id = i; barbero->seed = rand_r(&seed);
    pthread_create(&newThread, NULL, barberoFunc, barbero);
  }

  while (true) {
    ClienteInfo_T* cliente = (ClienteInfo_T*) malloc(sizeof(ClienteInfo_T));
    cliente->id = nextId; cliente->seed = rand_r(&seed);
    nextId++;
    pthread_create(&newThread, NULL, clienteFunc, cliente);
    //espera
    waitingTime = (rand_r(&seed) % 5) + 1;
    sleep(waitingTime * TICK);
  }
  return 0;
}
