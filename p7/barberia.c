#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

//malloc, free, rand_r, time
#include <stdlib.h>
#include <time.h>

//sleep
#include <unistd.h>

/*

3 barberos, 3 sillas de barberos
sofa 4 plazas
sala espera de pie
caja cobrar

si la barberia está llena, no pueden entrar más clientes la capacidad es 20

al entrar los clientes toman asiento o se quedan de pie

cuando un barbero queda libre coge al cliente que lleva mas tiempo esperando en el sofa y el cliente que esta de pie mas tiempo toma asiento en el sofa

al finalizar el corte de pelo cualquier barbero puede aceptar el pago, pero solo 1 caja a la vez

los barberos dividen su tiempo entre cortar el pelo, aceptar pagos y dormir en su silla esperando clientes

la barberia cerrara si no quedan clientes

*/

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

/*pthread_mutex_lock(&mutex) pthread_mutex_unlock(&mutex)*/
//mutex silla
pthread_mutex_t cliente_listo = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t dejar_silla_barbero = PTHREAD_MUTEX_INITIALIZER;
//mutex corte de pelo
pthread_mutex_t terminado= PTHREAD_MUTEX_INITIALIZER;
//mutex caja registradora
pthread_mutex_t pago = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t recibo = PTHREAD_MUTEX_INITIALIZER;

/*
https://www.dlsi.ua.es/asignaturas/pc/p05.html
https://www.dlsi.ua.es/asignaturas/pc/p07.html
gcc -o barberia barberia.c -lpthread
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
  while(true) {
    //sem_wait(&pago);
    //sem_wait(&coord);
    //cobrar();
    //sem_post(&coord);
    //sem_post(&recibo);
  }
}

void *barberoFunc(void *arg) {
  BarberoInfo_T barbero = *(BarberoInfo_T *) arg;
  free(arg);

  while(true) {
    //sem_wait(&cliente_listo);
    //sem_wait(&coord);
    //cortar_pelo();
    //sem_post(&coord);
    //sem_post(&terminado);
    //sem_wait(&dejar_silla_barbero)
    //sem_post(&silla_barbero)
  }
}

void *clienteFunc (void *arg) {
  //crea una copia local y libera la memoria reservada
  ClienteInfo_T cliente = *(ClienteInfo_T *) arg;
  free(arg);
  int waitingTime;

  printf("CLIENTE %d ENTRA EN ESCENA\n", cliente.id);

  sem_wait(&max_capacidad);
  waitingTime = (rand_r(&cliente.seed) % 2) + 1;
  sleep(waitingTime * TICK);
  printf("CLIENTE %d ENTRA A LA BABERIA\n", cliente.id);

  sem_wait(&sofa);
  waitingTime = (rand_r(&cliente.seed) % 2) + 1;
  sleep(waitingTime * TICK);
  printf("CLIENTE %d SE SIENTA EN EL SOFA\n", cliente.id);
  
  //sem_wait(&silla_barbero)
  //levantarse_sofa()
  
  waitingTime = (rand_r(&cliente.seed) % 2) + 1;
  sleep(waitingTime * TICK);
  printf("CLIENTE %d SE LEVANTA DEL SOFA\n", cliente.id);
  sem_post(&sofa);
  //sentarse_silla_barbero()
  //unlock(&cliente_listo)
  //lock(&terminado)
  //levantarse_silla_barbero()
  //unlock(&dejar_silla_barbero)
  //pagar()
  //unlock(&pago)
  //lock(&recibo)
  //salir_tienda()

  waitingTime = (rand_r(&cliente.seed) % 2) + 1;
  sleep(waitingTime * TICK);
  printf("CLIENTE %d SE VA DE LA BARBERIA\n", cliente.id);
  sem_post(&max_capacidad);

  pthread_exit(NULL);
}

int main () {
  unsigned int seed = time(NULL);
  unsigned int nextId = 0;
  int waitingTime, i;
  pthread_t newThread; // ignoramos los id de los threads (no haremos join)

  //inicializamos semaforos
  sem_init(&max_capacidad, 0, TAM_TOTAL);
  sem_init(&sofa, 0, TAM_SOFA);
  sem_init(&silla_barbero, 0, NUM_SILLAS);
  sem_init(&coord, 0, NUM_BARBEROS);

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
