#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include "circular_buffer.h"
#include <errno.h>
#include <string.h>
#include <signal.h>


// Global vars for free_resources
int shm_fd;
SharedBuffer *shared_buffer;
sem_t *res_free, *used;

void error_hanlde(void);
int read_value(SharedBuffer *shared_buffer, sem_t *res_free, sem_t *used);
void free_resources(void);
void print_semaphore_value(sem_t *sem);


void error_handle(void) {
	fprintf(stdout, "An error occured: %s\n", strerror(errno));
	free_resources();
	exit(EXIT_FAILURE);
}

void print_semaphore_value(sem_t *sem) {
	int sval;
	if (sem_getvalue(sem, &sval) == -1)
		debug("Couldn't get sempahore value. Error: %s", strerror(errno));
	else
		printf("Semaphore value: %d\n", sval);
}

int read_value(SharedBuffer *shared_buffer, sem_t *res_free, sem_t *used) {
	debug("Waiting used semaphore");
	print_semaphore_value(used);
	if (sem_wait(used) == -1) error_handle();
	/* The reader process will wait on the used semaphore before reading.
	   If used is 0, it means there is no data availabe to read, so the
	   reader will block until there is data */
	int val = shared_buffer->buf[shared_buffer->rd_pos];
	shared_buffer->rd_pos = (shared_buffer->rd_pos + 1) % BUF_LEN;
	debug("Posting free semaphore");
	print_semaphore_value(res_free);
	if (sem_post(res_free) == -1) error_handle();
	debug("Free semaphore after posting:"); print_semaphore_value(res_free);
	return val;
}

void free_resources(void) {
	debug("Starting to free resources...");
	if (munmap(shared_buffer, sizeof(SharedBuffer)) == -1)
		debug("Failed to unmap shared buffer. Error: %s", strerror(errno));
	if (close(shm_fd) == -1)
		debug("Failed to close shared memory file descriptor. Error: %s", strerror(errno));
	if (sem_close(res_free) == -1)
		debug("Failed to close free semaphore. Error: %s", strerror(errno));
	if (sem_close(used) == -1)
		debug("Failed to close used semaphore. Error: %s", strerror(errno));
	exit(0);
}

int main(void) {
	debug("Opening SHARED_MEM_NAME %s\n", SHARED_MEM_NAME);
	int shm_fd = shm_open(SHARED_MEM_NAME, O_RDONLY, 0666);
	if (shm_fd == -1) error_handle();

	debug("Mapping shared memory", NULL);
	SharedBuffer *shared_buffer = mmap(0, sizeof(SharedBuffer), PROT_READ, MAP_SHARED, \
		shm_fd, 0);
	if (shared_buffer == MAP_FAILED) error_handle();

	debug("Opening semaphores");
	sem_t *res_free = sem_open(SEMAPHORE_FREE_NAME, 0);
	sem_t *used = sem_open(SEMAPHORE_USED_NAME, 0);
	if (res_free == SEM_FAILED) {
		debug("res_free: %d, used: %d", res_free, used);
		error_handle();
	}

	void handle_signal(int signal) {
		debug("SIGINT received. Freeing resources");
		free_resources();
		exit(0);
	}

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handle_signal;
	sigaction(SIGINT, &sa, NULL);

	debug("Starting reader cycle");
	for (int i=0; i<10; i++) {
		int val = read_value(shared_buffer, res_free, used);
		printf("Reader: Read %d from buffer\n", val);
		sleep(1);
	}

	free_resources();

	return 0;
}
