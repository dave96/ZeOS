/*
 * fifo.c - Implements functions for handling a circular FIFO buffer.
 */

#include <fifo.h>
#include <types.h>

void fifo_init(fifo * b) {
	b->read_p = 0;
	b->write_p = 0;
	b->full = 0;
}

// Pre: fifo is not empty.
char fifo_read(fifo * b) {
	b->full = 0;
	char r = b->buffer[b->read_p];
	fifo_incr(b->read_p);
	return r;
}

void fifo_write(fifo * b, char w) {
	if (b->full) return;
	b->buffer[b->write_p] = w;
	fifo_incr(b->write_p);
	if (b->write_p == b->read_p) b->full = 1;
}
