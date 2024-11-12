#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "circular_buffer.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void error_handle(void) {
	fprintf(stdout, "An error occured: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
}

void write_value(SharedBuffer *shared_buffer, sem_t *free, sem_t *used, int val) {
	if (sem_wait(free) == -1) error_handle();
	/* If free reaches 0, it means the buffer is full, and any additional
	   attempt by the writer to sem_wait(free) will block until a reader
	   frees up a slot by reading a value and calling sem_post(free) */
	shared_buffer->buf[shared_buffer->wr_pos] = val;
	shared_buffer->wr_pos = (shared_buffer->wr_pos + 1) % BUF_LEN;
	if (sem_post(used) == -1) error_handle();
}



int main(int argc, char **argv) {

	// Set up shared memory for the circ buff
	int shm_fd = shm_open("/shared_circular_buffer", O_CREAT | O_RDWR, 0666);
	if (shm_fd == -1) error_handle();
	
	if (ftruncate(shm_fd, sizeof(SharedBuffer)) < 0) error_handle();
	SharedBuffer *shared_buffer = mmap(0, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, \
		MAP_SHARED, shm_fd, 0);
	if (shared_buffer == MAP_FAILED) error_handle();

	sem_t *free = sem_open(SEMAPHORE_FREE_NAME, O_CREAT, 0666, BUF_LEN); // all slots free
	sem_t *used = sem_open(SEMAPHORE_USED_NAME, O_CREAT, 0666, 0); // no used
	if (free == SEM_FAILED || used == SEM_FAILED) error_handle();

	for (int i=1; i<=10; i++) {
		printf("Writer: writing %d to buffer\n", i);
		write_value(shared_buffer, free, used, i);
		sleep(1);
	}

	if (munmap(shared_buffer, sizeof(SharedBuffer)) == -1) error_handle();
	if (close(shm_fd) == -1) error_handle();
	if (sem_close(free) == -1) error_handle();
	if (sem_close(used) == -1) error_handle();

	return 0;
}
