#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include "circular_buffer.h"
#include <errno.h>
#include <string.h>

void error_handle(void) {
	fprintf(stdout, "An error occured: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
}

int read_value(SharedBuffer *shared_buffer, sem_t *free, sem_t *used) {
	if (sem_wait(used) == -1) error_handle();
	/* The reader process will wait on the used semaphore before reading.
	   If used is 0, it means there is no data availabe to read, so the 
	   reader will block until there is data */
	int val = shared_buffer->buf[shared_buffer->rd_pos];
	shared_buffer->rd_pos = (shared_buffer->rd_pos + 1) % BUF_LEN;
	if (sem_post(free) == -1) error_handle();
	return val;
}


int main(void) {
	int shm_fd = shm_open(SHARED_MEM_NAME, O_RDONLY, 0666);
	if (shm_fd == -1) error_handle();

	SharedBuffer *shared_buffer = mmap(0, sizeof(SharedBuffer), PROT_READ, MAP_SHARED, \
		shm_fd, 0);
	if (shared_buffer == MAP_FAILED) error_handle();

	sem_t *free = sem_open(SEMAPHORE_FREE_NAME, O_CREAT, 0666, BUF_LEN);
	sem_t *used = sem_open(SEMAPHORE_USED_NAME, O_CREAT, 0666, 0);
	if (free == SEM_FAILED || used == SEM_FAILED) error_handle();

	for (int i=0; i<10; i++) {
		int val = read_value(shared_buffer, free, used);
		printf("Reader: Read %d from buffer\n", val);
		sleep(1);
	}

	if (munmap(shared_buffer, sizeof(SharedBuffer)) == -1) error_handle();
	if (close(shm_fd) == -1) error_handle();
	if (sem_close(free) == -1) error_handle();
	if (sem_close(used) == -1) error_handle();

	return 0;
}
