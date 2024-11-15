#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

#define SEM_1 "/sem_1"
#define SEM_2 "/sem_2"

int main(int argc, char **argv) {
	sem_t *s1 = sem_open(SEM_1, O_CREAT | O_EXCL, 0600, 1);
	sem_t *s2 = sem_open(SEM_2, O_CREAT | O_EXCL, 0600, 0);

	for (int i = 0; i < 3; i++) {
		sem_wait(s1);
		printf("Critical: %s: i = %d\n", argv[0], i);
		sem_post(s2);
	}
	sem_close(s1); sem_close(s2);
	return 0;
}
