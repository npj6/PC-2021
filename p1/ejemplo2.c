/* ejemplo1 */
//printf
#include <stdio.h>
//exit
#include <stdlib.h>
//wait
#include <sys/types.h>
#include <sys/wait.h>
//fork
#include <unistd.h>

#define PROC_NUM 3
#define PRINT_TIMES 5

void print_char(char c) {
	int i;
	for(i=0; i<PRINT_TIMES; i++)
		printf("%c", c);
}

void proc_code(char c) {
	print_char(c);
	exit(0);
}

int main() {
	int pid, p;

	int c[PROC_NUM] = {'A', 'B', 'C'};
	for(p=0; p<PROC_NUM; p++) {
		pid = fork();
		if (pid == 0) {
			proc_code(c[p]);
		}
	}

	for(p=0; p<PROC_NUM; p++) {
		wait(&pid);
	}
	return 0;
}
