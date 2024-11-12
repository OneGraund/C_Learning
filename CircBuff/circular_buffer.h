#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <semaphore.h>

#define BUF_LEN 8
#define SHARED_MEM_NAME "/shared_circ_buff"
#define SEMAPHORE_FREE_NAME "/free_slots"
#define SEMAPHORE_USED_NAME "/used_slots"

#ifdef DEBUG
	#define debug(fmt, ...) \
		(void) fprintf(stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
	#define debug(msg, ...) \
		(void) ;
#endif

typedef struct {
	int buf[BUF_LEN];
	int wr_pos;
	int rd_pos;
} SharedBuffer;

#endif
