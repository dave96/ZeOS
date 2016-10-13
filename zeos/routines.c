#include <routines.h>
#include <interrupt.h> // setInterruptHandler
#include <zeos_interrupt.h> // zeos_show_clock
#include <io.h> // inb, printc_xy
#include <devices.h>
#include <sched.h>
#include <handlers.h>
#include <utils.h>
#include <segment.h>

void keyboard_routine() {
	// DEBUG
	if (current() != (union task_union *) idle_task) task_switch((union task_union *) idle_task);
	else task_switch(&task[1]);
	
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
	zeos_ticks++;
	zeos_show_clock();
}

int sys_write(int fd, char * buffer, int size) {
	// Check fd
	int err = check_fd(fd, ESCRIPTURA);
	if (err < 0) return err;
	
	// Check buffer address
	if (buffer == NULL) return -EFAULT;

	// Check size
	if (size < 0) return -EINVAL;

	// UPDATE -> COPY TO USER WITH FIXED SIZE BUFFER !important
	return sys_write_console(buffer, size);
}

int sys_gettime() {
	return zeos_ticks;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int check_fd(int fd, int permissions)
{
  if (fd != 1) return -9; /*EBADF*/
  if (permissions != ESCRIPTURA) return -13; /*EACCES*/
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
