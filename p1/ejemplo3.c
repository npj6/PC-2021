/*ejemplo3*/
//printf, fprintf
#include <stdio.h>
//pthread
#include <pthread.h>
//exit
#include <stdlib.h>
//sterror
#include <string.h>


#define THREAD_NUM 3

typedef struct {
	char c;
	int times;
} C_TIMES_T;

void print_c_times(C_TIMES_T s) {
	int i;
	for (i=0; i<s.times; i++)
		printf("%c", s.c);
	pthread_exit(0);
}

void print_error(int error) {
	fprintf(stderr, "Error: %d, %s\n", error, strerror(error));
}

void *proc_code (void *s) {
	print_c_times(*(C_TIMES_T *)s);
	pthread_exit(0);
}

int main() {
	int t, error;
	int *salida;
	pthread_t threads[THREAD_NUM];
	C_TIMES_T s[THREAD_NUM] = { {'A', 50}, {'B', 100}, {'C', 150} };

	for(t=0; t<THREAD_NUM; t++) {
		error = pthread_create(&threads[t], NULL, proc_code, &s[t]);
		if (error) { print_error(error); exit(-1); }
	}
	
	for(t=0; t<THREAD_NUM; t++) {
		error = pthread_join(threads[t], (void**)&salida);
		if (error) print_error(error);
	}
	return 0;
}
