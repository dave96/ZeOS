#include <routines.h>
#include <interrupt.h> // setInterruptHandler
#include <zeos_interrupt.h> // zeos_show_clock
#include <io.h> // inb, printc_xy
#include <devices.h>
#include <sched.h>
#include <handlers.h>
#include <utils.h>
#include <mm.h>
#include <segment.h>

char syscall_buffer[100];

void keyboard_routine() {
	stats_enter_kernel();
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

	stats_exit_kernel();
	return;
}

void clock_routine() {
	stats_enter_kernel();
	zeos_ticks++;
	zeos_show_clock();
	schedule();
	stats_exit_kernel();
}

int sys_write(int fd, char * buffer, int size) {
	stats_enter_kernel();
	// Check fd
	int err = check_fd(fd, ESCRIPTURA);
	if (err < 0) return err;
	
	// Check buffer address
	if (buffer == NULL) return -EFAULT;
	if (!access_ok(VERIFY_READ, buffer, size)) return -EFAULT;

	// Check size
	if (size < 0) return -EINVAL;

	// Copy small buffers and write them to console.
	int rsize;
	for(rsize = size; rsize >= 100; rsize -= 100) {
		copy_from_user(buffer + (size - rsize), syscall_buffer, 100);
		sys_write_console(syscall_buffer, 100);
	}
	if (rsize > 0) {
		copy_from_user(buffer + (size - rsize), syscall_buffer, rsize);
		sys_write_console(syscall_buffer, rsize);
	}
	
	stats_exit_kernel();
	return size;
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
	stats_enter_kernel();
	
	int PID = get_new_pid();

	// Try to create a new task_struct. If no free task structs, return error.

	if (list_empty(&freequeue)) return -ENOMEM;

	// There are availible task structs. We will only remove it from the list if we can allocate memory later.

	struct list_head *freeHead = list_first(&freequeue);
	struct task_struct *newTask = list_head_to_task_struct(freeHead);
	
	struct list_head anchor = newTask->list;

	// Copy the parent task_union to the child.

	copy_data(current(), newTask, KERNEL_STACK_SIZE*4);
	
	// Restore list anchor!
	
	newTask -> list = anchor;

	// Allocate directory for child

	allocate_DIR(newTask);

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

	page_table_entry *child = get_PT(newTask);
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
		copy_data((void * ) ((PAG_LOG_INIT_DATA+pg)*PAGE_SIZE), (void *) ((PAG_LOG_INIT_DATA + NUM_PAG_DATA + pg)*PAGE_SIZE), PAGE_SIZE);
		del_ss_pag(parent, PAG_LOG_INIT_DATA + NUM_PAG_DATA + pg);
	}
	
	// Flush TLB
	set_cr3(get_DIR(current()));
	
	// After copying, we have to assign a new PID
	newTask->PID = PID;
	
	// Stats are being tracked.
	newTask->status = STATUS_ALIVE;
	
	// Now, the ESP for the new process. We will set this in order to simulate a task_switch call.
	// EBP <- @RET(RET_FROM_FORK) <- @RET(SYSCALL HANDLER) <- EXECUTION CONTEXT
	newTask->esp = KERNEL_ESP((union task_union *) newTask) - sizeof(struct execution_context) - 12;
	
	((union task_union *) newTask)->stack[KERNEL_STACK_SIZE-16-2] = (unsigned long) &ret_from_fork; // @RET(RET_FROM_FORK)
	((union task_union *) newTask)->stack[KERNEL_STACK_SIZE-16-3] = newTask->esp + 4; // EBP
	
	// Insert the process into the ready queue;
	list_del(freeHead);
	list_add_tail(&(newTask->list), &readyqueue);
	
	// Init stats for process
	init_stats(newTask);
	
	// Return child PID
	stats_exit_kernel();
	return PID;
}

void sys_exit() {
	// Free memory if dir empty.
	if (--*(get_DIR_alloc(current())) == 0)	free_user_pages(current());
	// No podemos pedir stats.
	current()->status = STATUS_DEAD;
	// Free task_struct.
	list_add_tail(&(current()->list), &freequeue);
	// Scheduling
	sched_next_rr();
}

int sys_getstats(int pid, struct stats *t) {
	stats_enter_kernel();
	// Check memory.
	if (t == NULL) return -EFAULT;
	if (!access_ok(VERIFY_WRITE, t, sizeof(struct stats))) return -EFAULT;
	
	// Check PID exists
	if (pid < 0) return -EINVAL;
	if (pid > PID_MAX) return -ESRCH;
	struct stats * st = get_stats_pid(pid);
	if (st == NULL) return -ESRCH;
	
	// stats and pid exists. Copy stats and return 0.
	copy_to_user(st, t, sizeof(struct stats));
	stats_exit_kernel();
	return 0;
}

int sys_clone (void (*function)(void), void *stack) {
	int PID = get_new_pid();

	// Try to create a new task_struct. If no free task structs, return error.

	if (list_empty(&freequeue)) return -ENOMEM;
	
	// Check if stack is a valid location, and if function is inside the code area.
	
	if (!access_ok(VERIFY_WRITE, stack, 4)) return -EINVAL;
	if (!access_ok(VERIFY_READ, function, 4)) return -EINVAL;

	// There are availible task structs. We will only remove it from the list if we can allocate memory later.

	struct list_head *freeHead = list_first(&freequeue);
	struct task_struct *newTask = list_head_to_task_struct(freeHead);
	
	struct list_head anchor = newTask->list;

	// Copy the parent task_union to the child.

	copy_data(current(), newTask, KERNEL_STACK_SIZE*4);
	
	// Restore list anchor!
	
	newTask->list = anchor;

	// No copying or allocation is done, as both of them share pages.
	++*(get_DIR_alloc(current()));
	
	// After copying, we have to assign a new PID
	newTask->PID = PID;
	
	// Stats are being tracked.
	newTask->status = STATUS_ALIVE;
	
	// Now, the ESP for the new process. We will set this in order to simulate a task_switch call.
	// EBP <- @RET(RET_FROM_FORK) <- @RET(SYSCALL HANDLER) <- EXECUTION CONTEXT
	newTask->esp = KERNEL_ESP((union task_union *) newTask) - sizeof(struct execution_context) - 12;
	
	((union task_union *) newTask)->stack[KERNEL_STACK_SIZE-16-2] = (unsigned long) &ret_from_fork; // @RET(RET_FROM_FORK)
	((union task_union *) newTask)->stack[KERNEL_STACK_SIZE-16-3] = newTask->esp + 4; // EBP
	
	// Now, set the CHILD ESP to point to the correct position, as specified in arguments
	
	((union task_union *) newTask)->stack[KERNEL_STACK_SIZE-2] = (DWord) stack; // ESP
	((union task_union *) newTask)->stack[KERNEL_STACK_SIZE-5] = (DWord) function; //EIP
	
	// Insert the process into the ready queue;
	list_del(freeHead);
	list_add_tail(&(newTask->list), &readyqueue);
	
	// Init stats for process
	init_stats(newTask);
	
	// Return thread PID
	return PID;
}

int sys_sem_init (int n_sem, unsigned int value) {
	
}

int sys_sem_wait (int n_sem) {
}

int sys_sem_signal (int n_sem) {
}

int sys_sem_destroy (int n_sem) {
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
