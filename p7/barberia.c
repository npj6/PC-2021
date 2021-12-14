#include <stdio.h>
#include <pthread.h>

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

/*
sem_t

  max_capacidad, sofa, silla_barbero

  coord (ocupacion de los barberos, para cortar el pelo y para cobrar)

mutex
  cliente_listo (cliente sentado en silla)

  terminado (corte de pelo terminado)

  dejar_silla_barbero (cliente abandona silla tras corte de pelo)

  pago (controla el pago)
  recibo (entrega al cliente un recibe de pago)

*/

/*
https://www.dlsi.ua.es/asignaturas/pc/p05.html
https://www.dlsi.ua.es/asignaturas/pc/p07.html
gcc -o barberia barberia.c -lpthread
*/
#define true 1
#define false 0

typedef struct {
  unsigned int id, seed;
} ClienteInfo_T;

void *clienteFunc (void *arg) {
  ClienteInfo_T cliente = *(ClienteInfo_T *) arg;
  free(arg); //crea una copia local y libera la memoria reservada
  
  printf("CLIENTE %d SEED %d\n", cliente.id, cliente.seed);
  //wait(max_capacidad)
  //entrar_barberia()
  //wait(sofa)
  //sentarse_sofa()
  //wait(silla_barbero)
  //levantarse_sofa()
  //signal(sofa)
  //sentarse_silla_barbero()
  //signal(cliente_list)
  //wait(terminado)
  //levantarse_silla_barbero()
  //signal(dejar_silla_barbero)
  //pagar()
  //signal(pago)
  //wait(recibo)
  //salir_tienda()
  //signal(max_capacidad)
}

int main () {
  unsigned int seed = time(NULL);
  unsigned int nextId = 0;
  int waitTime;

  pthread_t newThread; // ignoramos el id del thread cliente (no haremos join)
  
  while (true) {
    ClienteInfo_T* cliente = (ClienteInfo_T*) malloc(sizeof(ClienteInfo_T));
    cliente->id = nextId; cliente->seed = rand_r(&seed);
    nextId++;
    pthread_create(&newThread, NULL, clienteFunc, cliente);
    waitTime = (rand_r(&seed) % 5) + 1;
    sleep(waitTime);
  }
  return 0;
}
