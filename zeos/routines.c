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
		// DEBUG
		if (current() != (union task_union *) idle_task) task_switch((union task_union *) idle_task);
		else task_switch(&task[1]);
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

int sys_getpid() {
	return current()->PID;
}

int sys_gettime() {
	return zeos_ticks;
}

int sys_ni_syscall() {
	return -38; /*ENOSYS*/
}

int sys_fork() {
	int PID=-1;

	// Try to create a new task_struct. If no free task structs, return error.

	if (list_empty(&freequeue)) return -ENOMEM;

	// There are availible task structs. We will only remove it from the list if we can allocate memory later.

	struct list_head *freeHead = list_first(&freequeue);
	/*list_del(freeHead);*/

	// Copy the parent task_union to the child.

	copy_data(current(), freeHead, KERNEL_STACK_SIZE*4);

	// Allocate directory for child

	allocate_DIR(freeHead);

	// Try to allocate data pages.

	unsigned int user_pages[NUM_PAG_DATA];
	int pg;

	for (pg = 0; pg < NUM_PAG_DATA; ++pg) {
	user_pages[pg] = alloc_frame();
		if (user_pages[pg] < 0) {
			for(pg = pg-1; pg >= 0; --pg) free_frame(user_pages[pg]);
			return -ENOMEM;
		}
	}

	// Get the page tables.

	page_table_entry *child = get_PT(freeHead);
	page_table_entry *parent = get_PT(current());
  
	// We have enough memory for the user data. Now map the frames to the logical part.
	
	// CODE
	for (pg=0; pg < NUM_PAG_CODE; pg++)
		set_ss_pag(child, PAG_LOG_INIT_CODE+pg, get_frame(parent, PAG_LOG_INIT_CODE+pg));
	
	// DATA
	for (pg=0; pg < NUM_PAG_DATA; pg++)
		set_ss_pag(child, PAG_LOG_INIT_DATA+pg, user_pages[pg]);
	
	// Now, let's copy the data from the parent to the child. We have to map temporal pages in the parent
	// page table, copy and unmap.
	
	for (pg=0; pg < NUM_PAG_DATA; pg++) {
		set_ss_pag(parent, PAG_LOG_INIT_DATA + NUM_PAG_DATA + pg, user_pages[pg]);
		copy_data(
	}
	
	
}
  
  
  // creates the child process
  
  return PID;
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
