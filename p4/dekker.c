/*
 * hilos
 * Compilación: cc -o hilos hilos.c -lpthread
 */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define false 0
#define true 1

#define VALUE 200000000

int cs_ready[2] = {false, false};
int turn = 0;

void pre_cs(int yo, int otro) {
  cs_ready[yo] = true;
  while (cs_ready[otro]) {
    if (turn == otro) {
      cs_ready[yo] = false;
      while (turn != yo);
      cs_ready[yo] = true;
    }
  }
}

void post_cs(int yo, int otro) {
  cs_ready[yo] = false;
  turn = otro;
}
 
int I = 0;

void *codigo_del_hilo (void *id){
  int yo =  *(int *)id;
  int otro = 1 - yo;
  
  pre_cs(yo, otro);
  
  int i;
  for(i=0; i<VALUE; ++i)
    ++I;
  
  post_cs(yo, otro);
  
  pthread_exit (id);
}

void manejar_error (int error) {
  if (error) {
    fprintf (stderr, "Error: %d: %s\n", error, strerror (error));
    exit(-1);
  }
}

int main(){
   pthread_t hilos[2];
   int id[2] = {0, 1};
   int error;
   int *salida;

   error = 0;
   error &= pthread_create( &hilos[0], NULL, codigo_del_hilo, &id[0]);
   error &= pthread_create( &hilos[1], NULL, codigo_del_hilo, &id[1]);
   manejar_error(error);
   
   error = 0;
   error &= pthread_join(hilos[0], (void **)&salida);
   error &= pthread_join(hilos[1], (void **)&salida);
   manejar_error(error);
   
   printf("I vale %d, deberia valer %d\n", I, VALUE*2);
   
}
