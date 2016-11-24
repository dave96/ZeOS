/*
 * fifo.h - headers for a circular FIFO buffer
 */
#include <types.h>
 
#ifndef __FIFO_H__
#define __FIFO_H__

#define FIFO_SIZE 50

#define fifo_full(t)	(t.full)
#define fifo_incr(t)	t = (t + 1) % FIFO_SIZE 
#define fifo_empty(t)	((!(t.full)) && (t.read_p == t.write_p))

typedef struct {
	unsigned int read_p;
	unsigned int write_p;
	unsigned char full;
	char buffer[FIFO_SIZE];
} fifo;

void fifo_init(fifo * b);
char fifo_read(fifo * b);
void fifo_write(fifo * b, char w);

#endif  /* __FIFO_H__ */
