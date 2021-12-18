/*gcc -o barberia barberia.c -lpthread*/
/*VER MEMORIA*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

//malloc, free, rand_r, time
#include <stdlib.h>
#include <time.h>

//sleep
#include <unistd.h>

#define true 1
#define false 0

typedef struct {
  unsigned int id, seed;
} ClienteInfo_T;

typedef ClienteInfo_T BarberoInfo_T;

#define TAM_TOTAL 20
#define TAM_SOFA 4
#define NUM_SILLAS 3
#define NUM_BARBEROS 3

// las esperas se realizan en ticks en lugar de segundos
// la constante TICK define el numero de segundos de un tick
#define TICK 3

#define BORRAR_COLOR "\033[0m"
#define CAJERO_COLOR "\033[36m"
#define CLIENTE_COLOR "\033[33m"
#define BARBERO_COLOR "\033[31m"

//ambos inclusive
#define CAJERO_MAX_T 5
#define CAJERO_MIN_T 1

#define BARBERO_MAX_T 7
#define BARBERO_MIN_T 1

#define CLIENTE_MAX_T 2
#define CLIENTE_MIN_T 1

#define APARECE_MAX_T 5
#define APARECE_MIN_T 1

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

void cajeroPrintln(const char * text) {
  printf("%sCAJERO %s\n%s", CAJERO_COLOR, text, BORRAR_COLOR);
}

void clientePrintln(int id, const char * text) {
  printf("%sCLIENTE %d %s\n%s", CLIENTE_COLOR, id, text, BORRAR_COLOR);
}

void barberoPrintln(int id, const char * text) {
  printf("%sBARBERO %d %s\n%s", BARBERO_COLOR, id, text, BORRAR_COLOR);
}

void espera(unsigned int * seed, int min, int max) {
  int waitingTime = rand_r(seed) % (max-min+1);
  waitingTime = waitingTime + min;
  sleep(waitingTime * TICK);
}

void cajeroEspera(unsigned int * seed) {
  espera(seed, CAJERO_MIN_T, CAJERO_MAX_T);
}

void clienteEspera(unsigned int * seed) {
  espera(seed, CLIENTE_MIN_T, CLIENTE_MAX_T);
}

void barberoEspera(unsigned int * seed) {
  espera(seed, BARBERO_MIN_T, BARBERO_MAX_T);
}

void *cajeroFunc(void *arg) {
  unsigned int seed = *(unsigned int *) arg;
  while(true) {
    cajeroPrintln("ESPERANDO PAGO");
    sem_wait(&pago);

    cajeroPrintln("ESPERANDO BARBERO LIBRE");
    sem_wait(&coord);

    cajeroPrintln("REALIZANDO TRANSACCION");
    cajeroEspera(&seed);

    cajeroPrintln("HA TERMINADO TRANSACCION");
    sem_post(&coord);
    sem_post(&recibo);
  }
}

void *barberoFunc(void *arg) {
  //crea una copia local y libera la memoria reservada
  BarberoInfo_T barbero = *(BarberoInfo_T *) arg;
  free(arg);

  while(true) {
    barberoPrintln(barbero.id, "ESPERANDO CLIENTE");
    sem_wait(&cliente_listo);

    sem_wait(&coord);
    barberoPrintln(barbero.id, "CORTANDO PELO");
    barberoEspera(&barbero.seed);

    barberoPrintln(barbero.id, "HA ACABADO DE CORTAR PELO");
    sem_post(&coord);
    sem_post(&terminado);
    
    barberoPrintln(barbero.id, "ESPERANDO A QUE LA SILLA SE LIBERE");
    sem_wait(&dejar_silla_barbero);
    sem_post(&silla_barbero);
    barberoPrintln(barbero.id, "TIENE LA SILLA LIBRE");
  }
}

void *clienteFunc (void *arg) {
  //crea una copia local y libera la memoria reservada
  ClienteInfo_T cliente = *(ClienteInfo_T *) arg;
  free(arg);

  clientePrintln(cliente.id, "ENTRA EN ESCENA");

  sem_wait(&max_capacidad);
  clientePrintln(cliente.id, "ENTRA A LA BABERIA");
  clienteEspera(&cliente.seed);

  sem_wait(&sofa);
  clientePrintln(cliente.id, "SE SIENTA EN EL SOFA");
  clienteEspera(&cliente.seed);
  
  sem_wait(&silla_barbero);
  clientePrintln(cliente.id, "SE LEVANTA DEL SOFA Y SE SIENTA EN UNA SILLA");
  clienteEspera(&cliente.seed);
  sem_post(&sofa);

  sem_post(&cliente_listo);
  clientePrintln(cliente.id, "ESTA RECIBIENDO UN CORTE DE PELO");
  sem_wait(&terminado);
  clientePrintln(cliente.id, "HA RECIBIDO SU CORTE DE PELO");

  clientePrintln(cliente.id, "SE LEVANTA DE LA SILLA");
  clienteEspera(&cliente.seed);
  sem_post(&dejar_silla_barbero);

  clienteEspera(&cliente.seed);
  sem_post(&pago);

  clientePrintln(cliente.id, "ESPERANDO RECIBO");
  sem_wait(&recibo);

  clienteEspera(&cliente.seed);
  clientePrintln(cliente.id, "SE VA DE LA BARBERIA");
  sem_post(&max_capacidad);

  pthread_exit(NULL);
}

int main () {
  unsigned int seed = time(NULL);
  unsigned int nextId = 0;
  unsigned int cajeroSeed;
  int i;
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

  //crea thread cajero
  cajeroSeed = rand_r(&seed);
  pthread_create(&newThread, NULL, cajeroFunc, &cajeroSeed);

  for(i=0; i<NUM_BARBEROS; ++i) {
    //crea info barbero
    BarberoInfo_T* barbero = (BarberoInfo_T*) malloc(sizeof(BarberoInfo_T));
    barbero->id = i; barbero->seed = rand_r(&seed);
    //crea thread barbero
    pthread_create(&newThread, NULL, barberoFunc, barbero);
  }

  while (true) {
    //crea info cliente
    ClienteInfo_T* cliente = (ClienteInfo_T*) malloc(sizeof(ClienteInfo_T));
    cliente->id = nextId; cliente->seed = rand_r(&seed);
    nextId++;
    //crea thread cliente
    pthread_create(&newThread, NULL, clienteFunc, cliente);
    //espera
    espera(&seed, APARECE_MIN_T, APARECE_MAX_T);
  }
  return 0;
}
