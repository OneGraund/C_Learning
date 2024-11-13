
#ifndef HEADER_H
#define HEADER_H

#include "graph.h"

#define BUF_LEN 1024
#define SHARED_MEM_NAME "/shared_circ_buff"
#define SEMAPHORE_FREE_NAME "/free_slots"
#define SEMAPHORE_USED_NAME "/used_slots"

#ifdef DEBUG
	#define debug(fmt, ...) \
		(void) fprintf(stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
	#define debug(msg, ...)
#endif

typedef struct {
	Graph buf[BUF_LEN];
	int wr_pos;
	int rd_pos;
	bool should_terminate;
} SharedMemory;

bool is_string_numeric(const char *str);

#endif
