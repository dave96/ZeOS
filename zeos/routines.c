#include <routines.h>
#include <interrupt.h> // setInterruptHandler
#include <zeos_interrupt.h> // zeos_show_clock
#include <io.h> // inb, printc_xy
#include <device.h>
#include <handlers.h>

void keyboard_routine() {
	// ISR 33 - Key Press.
	unsigned char input = inb(__DATA_PORT);
	
	if((input & 0x80) == 0x00) {
		// MAKE
	input = (input & 0x7F);
		
		char input_char = char_map[input];
		if (input_char != '\0') {
			printc_xy(0, 0, input_char);
		} else {
			printc_xy(0, 0, 'C');
		}
	}
	return;
}

void clock_routine() {
	// Do some shit
	// zeos_show_clock();
}

int sys_write(int fd, char * buffer, int size) {
	// Check fd
	int err = check_fd(fd, ESCRIPTURA);
	if (err < 0) return err;
	
	// Check buffer
	if (buffer == NULL) 
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

void init_interrupt_handlers() {
	// Interrupciones
	setInterruptHandler(33, keyboard_handler, 0);
	setInterruptHandler(32, clock_handler, 0);
	
	// Llamadas a sistema
	setTrapHandler(0x80, system_call_handler, 3);
	
	// Excepciones
}