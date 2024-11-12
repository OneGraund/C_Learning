#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "circular_buffer.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#ifdef DEBUG
#define debug(fmt, ...) \
	(void) fprintf(stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(msg, ...)
#endif

int shm_fd;
SharedBuffer *shared_buffer;
sem_t *res_free, *used;

void error_handle(void) {
	debug("Launched error handling");
	fprintf(stdout, "An error occured: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
}

void write_value(SharedBuffer *shared_buffer, sem_t *res_free, sem_t *used, int val) {
	debug("Waiting res_free. Curr - %d", res_free);
	if (sem_wait(res_free) == -1) error_handle();
	/* If res_free reaches 0, it means the buffer is full, and any additional
	   attempt by the writer to sem_wait(res_free) will block until a reader
	   res_frees up a slot by reading a value and calling sem_post(res_free) */
    debug("Writing value to position %d", shared_buffer->wr_pos);
	shared_buffer->buf[shared_buffer->wr_pos] = val;
	shared_buffer->wr_pos = (shared_buffer->wr_pos + 1) % BUF_LEN;
	debug("Incremented position to %d and posting used", shared_buffer->wr_pos);
	if (sem_post(used) == -1) error_handle();
}

void free_resources(SharedBuffer *shared_buffer, sem_t *res_free, sem_t *used) {
	printf("Freeing resources\n");

	debug("Unmapping shread_buffer");
	if (munmap(shared_buffer, sizeof(SharedBuffer)) == -1) error_handle();

	debug("Closing res_free semaphore");
 	if (sem_close(res_free) == -1) error_handle();

 	debug("Closing used semaphore");
	if (sem_close(used) == -1) error_handle();

	debug("Unlinking shared memory");
	if (shm_unlink(SHARED_MEM_NAME) == -1) error_handle();

	debug("Unlinking free semaphore");
	if (shm_unlink(SEMAPHORE_FREE_NAME) == -1) error_handle();

	debug("Unlinking used semaphore");
	if (shm_unlink(SEMAPHORE_USED_NAME) == -1) error_handle();
}


int main(int argc, char **argv) {

	// Set up shared memory for the circ buff
	int shm_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
	if (shm_fd == -1) error_handle();

	if (ftruncate(shm_fd, sizeof(SharedBuffer)) < 0) error_handle();
	SharedBuffer *shared_buffer = mmap(0, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, \
		MAP_SHARED, shm_fd, 0);
	if (shared_buffer == MAP_FAILED) error_handle();
	if (close(shmfd) == -1) error_handle();

	sem_t *res_free = sem_open(SEMAPHORE_FREE_NAME, O_CREAT, 0666, BUF_LEN); // all slots free
	sem_t *used = sem_open(SEMAPHORE_USED_NAME, O_CREAT, 0666, 0); // no used
	if (res_free == SEM_FAILED || used == SEM_FAILED) error_handle();

	
	void handle_signal(int signal) {
		free_resources(shared_buffer, res_free, used);
		exit(0);
	}

	struct sigaction sa;
	memset(&sa,	0, sizeof(sa));
	sa.sa_handler = handle_signal;
	sigaction(SIGINT, &sa, NULL);

	for (int i=1; i<=10; i++) {
		printf("Writer: writing %d to buffer\n", i);
		write_value(shared_buffer, res_free, used, i);
		sleep(1);
	}

	free_resources(shared_buffer, res_free, used);

	return 0;
}
