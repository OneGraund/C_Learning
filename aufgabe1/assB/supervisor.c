/**
 * @file supervisor.c
 *
 * @brief
 *
 * @details
 *
 * @synopsis
 *		supervisor [-n limit] [-w delay]
 * @param -n  The argument limit specifies a limit (integer value) for the
 *			  number of generated solutions. If limit is omitted, it should
 * 			  be considered as infinite
 * @param -w  The argument delay specifies a delay (in seconds) before reading
 *			  the first solution from the buffer. If delay is omitted, it
 *		      should be considered as zero.
 *
 * @author Volodymyr Skoryi
 * @date 08.11.2024
 */


#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>

/**
 * @def DEBUG
 * @brief Enables debug mode for detailed error messages
 * @details When this file is compiled with a -DDEBUG flag, 'debug' macto is going to print
 *          debug information to stderr stream including file name and line number. In case
 *          this flag was not specified, than debug outputs are not going to be printed
 * @param fmt The format string, similar to printf
 * @param ... Additional arguments to plug in the string
 */

#ifdef DEBUG
#define debug(fmt, ...) \
	(void) fprintf(stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(msg, ...)
#endif

#define SHM_NAME "/supervisor_generator_shm"
#define MAX_DATA (4096)

typedef struct shm {
	unsigned int state;
	unsigned int data[MAX_DATA];
} shm_t;


void handle_error(const char *msg);
void terminate_generators(const char *message);
int create_shared_memory(void);
shm_t* map_shared_memory(int shmfd);
void release_shared_memory(shm_t *shm);
bool is_string_numeric(const char *str);


/**
 * @brief Prints the usage message for the 3-Coloring utility
 *
 * @details This function outputs detailed synopsis of the command-line options
 *		    and arguments for the utility to stderr stream, then exits the
 *   	    program with a failure code EXIT_FAILURE.
 *
 * @author Volodymyr Skoryi
 * @date   08.11.2024
 */
void usage(void) {
	fprintf(stderr, "Usage: \n\t supervisor [-n limit] [-w delay]\n");
	exit(EXIT_FAILURE);
}


int main(int argc, char* argv[]) {
	debug("Program started, fetching args", NULL);
	unsigned long lim = 0; // if lim is 0 it is considered as infin
	unsigned int delay = 0; // if 0 then 0

	char *str = NULL; // temporary string for str -> int conversion

	int c;
	while ( (c = getopt(argc, argv, "n:w:")) != -1 ) {
		switch (c) {
			case 'n':
				str = optarg;
				if (is_string_numeric(str) == true)
					lim = strtol(str, NULL, 10);
				else {
					debug("Specified lim is non-numeric. Got: %s", str);
					usage();
				}
				break;
			case 'w':
				str = optarg;
				if (is_string_numeric(str) == true)
					delay = strtol(str, NULL, 10);
				else {
					debug("Specified delay is non-numeric. Got: %s", str);
					usage();
				}
				break;
			case '?': usage();
				break;
		}
	}

	if (lim == 0) {
		debug("Because lim was not specified, it is set to ULONG_MAX");
		lim = ULONG_MAX - 1;
	}

	debug("Arguments that will be used: limit - %lu, delay - %d", lim, delay);

	// Set up the shared memory and the semaphores
	debug("Setting up the shared memory and the semaphores");
	int shared_memory = create_shared_memory();
	shm_t* memory_mapping = map_shared_memory(shared_memory);

		// Set up a variable, which is checked by generator processes (to terminate)
	// Initialize the circular buffer required for the communication with the generators
		// Keep reading until SIGINT/SIGTERM received or if [-n limit crossed]
			// Notify generators to terminate
			// Unlink all shared recources
			// Print to stdout -> The graph might not be 3-colorable, best solution
				// N edges. (N is replaced with the number of edges of the best solution)
			// Exit
	// Initialization complete -> wait for DELAY seconds and read solutions from circ buff
		// Note: Use sleep() to give generators enough time to start
	// Remember the best solution (e.g. solution with the least edges)
		// When a beter solution -> write to stderr
	// If generator writes solution with 0 edges to circ buff -> terminate
		// Print to stdout -> The graph is 3-colorable!
		// Exit
	return 0;
}


/**
 * @brief Checks if a string is numeric.
 */
bool is_string_numeric(const char *str) {
	if (*str == '\0')
		return false;

	while (*str) {
		if (!isdigit((unsigned char) *str))
			return false;
		str++;
	}

	return true;
}


/**
 * @brief Outputs message and current error description. Terminates execution of program.
 *
 * @param msg Message to be displayed to stderr stream.
 */

void handle_error(const char *msg) {
	char msg_error[strlen(msg) + 128];
	strncpy(msg_error, msg, sizeof(msg_error) - 1);
	msg_error[sizeof(msg_error) - 1] = '\0';
	// Use strncat to calculate left space in string
	strncat(msg_error, " Details: ", sizeof(msg_error) - strlen(msg_error) - 1);
	strncat(msg_error, strerror(errno), sizeof(msg_error) - strlen(msg_error) - 1);
	fprintf(stderr, "%s\n", msg_error);
	terminate_generators("Error handling");

	exit(EXIT_FAILURE);
}

/**
 * @brief Send termination signals to all running generators.
 *
 * @param message (Optional, may be NULL) Message to be dispalayed during debug.
 */
void terminate_generators(const char *message) {
	debug("TERMINATION OF GENERATORS IS NOT SET UP YET!!!!");
	exit(EXIT_FAILURE); // Remove after implementation
	if (message != NULL)
		debug("Termination of generators was requested, sending SIGTERMs signals...");
	else
		debug("Termination of generators was requested. Message: %s. Sending SIGTERMs...", \
			message);
	int pid;
	kill(pid, SIGTERM);
}

/**
 * @brief Create and size shared memory based on SHM_NAME and MAX_DATA in struct
 *
 * @return shared memory file descriptor
 */

int create_shared_memory(void) {
	int shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0600); // Owner permission
	if (shmfd == -1)
		handle_error("Failed to create shared memory.");

	// Set size of shared memory
	if (ftruncate(shmfd, sizeof(shm_t)) == -1)
		handle_error("Failed to set size for shared memory.");

	return shmfd;
}

/**
 * @brief Maps shared memory from file descriptor
 *
 * @return shm Starting address of the mapping
 */

shm_t* map_shared_memory(int shmfd) {
	shm_t *shm;
	shm = mmap(NULL, sizeof(*shm), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

	if (shm == MAP_FAILED)
		handle_error("Failed to map shared memory.");

	if (close(shmfd) == -1)
		handle_error("Failed to close shared memory file descriptor.");

	return shm;
}

/**
 * @brief Cleanup of shared memory. Unmapping and removing of object
 *
 * @param shm Shared memory structure object
 */
void release_shared_memory(shm_t *shm) {
	if (munmap(shm, sizeof(*shm)) == -1)
		handle_error("Failed to unmap shared memory");
	if (shm_unlink(SHM_NAME) == -1)
		handle_error("Failed to unlink shared memory");
}
