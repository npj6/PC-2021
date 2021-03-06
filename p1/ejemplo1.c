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

#define PROC_NUM 2

void print_int(int i) {
	printf("%d", i);
}

void proc_code(int i) {
	print_int(i);
	exit(0);
}

int main() {
	int pid, p;

	int i[PROC_NUM] = {1, 2};
	for(p=0; p<PROC_NUM; p++) {
		pid = fork();
		if (pid == 0) {
			proc_code(i[p]);
		}
	}

	for(p=0; p<PROC_NUM; p++) {
		wait(&pid);
	}
	return 0;
}
