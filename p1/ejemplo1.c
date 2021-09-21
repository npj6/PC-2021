/* ejemplo1 */
#include <stdio.h>
#include <stdlib.h>

#define PROC_NUM 2

void print_int(int i) {
	printf("%d", i);
}

int main() {
	int pid;

	int c[PROC_NUM] = {1, 2};
	for(p=0; p < PROC_NUM; p++) {
		pid = fork();
	}
	return 0;
}
