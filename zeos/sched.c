/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <list.h>
#include <handlers.h>

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}

extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}

int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

/* MULTITASK MANAGEMENT FUNCTIONS */

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
		printc('0' + (char) current()->PID);
	}
}

void init_idle (void)
{
	// Cogemos la primera entrada de la lista y la quitamos de ella.
	struct list_head *freeHead = list_first(&freequeue);
	list_del(freeHead);
	
	// Entramos al task_struct
	struct task_struct *new = list_head_to_task_struct(freeHead);

	// Asignamos al task_struct el PID 0
	new->PID = 0;
	
	// Asignamos un quantum de 1 para que el proceso no se quede en la cpu más de lo necesario.
	set_quantum(new, 1);
	
	// Allocateamos una nueva página para el proceso.
	allocate_DIR(new);
	
	// Definimos el contexto de restauración (ebp i @RET) y cambiamos esp.
	union task_union * t = (union task_union *) new;
	t->stack[KERNEL_STACK_SIZE - 2] = 0x666;
	t->stack[KERNEL_STACK_SIZE - 1] = (unsigned long) &cpu_idle;
	new->esp = (unsigned long) &(t->stack) + KERNEL_STACK_SIZE*4 - 8;
	
	// Definimos variable global
	idle_task = new;
}

void init_task1(void)
{
	// Cogemos la primera entrada de la lista y la quitamos de ella.
	struct list_head *freeHead = list_first(&freequeue);
	list_del(freeHead);
	
	// Entramos al task_struct
	struct task_struct *new = list_head_to_task_struct(freeHead);
	
	// Ponemos el quantum
	set_quantum(new, DEFAULT_QUANTUM);
	current_ticks_left = DEFAULT_QUANTUM;
	
	// Asignamos al task_struct el PID 1
	new->PID = 1;
	
	// Allocateamos una nueva página para el proceso.
	allocate_DIR(new);
	
	// Configuramos sus páginas de código y datos
	set_user_pages(new);
	
	// Update del TSS para apuntar a la system stack de new
	union task_union * t = (union task_union *) new;
	tss.esp0 = KERNEL_ESP(t);
	
	// Cambiamos cr3 al page directory de Task 1 y flush de TLB
	set_cr3(get_DIR(new));
}


void init_sched(){
	// Init list_head structures for queues.
	INIT_LIST_HEAD(&freequeue);
	INIT_LIST_HEAD(&readyqueue);
	
	// Set initial PID.
	current_pid = 100;
	
	// Place all task structures in the freequeue as no tasks are running.
	int i;
	for(i = 0; i < NR_TASKS; ++i) {
		list_add_tail(&(task[i].task.list), &freequeue);
	}
}

int get_new_pid(void) {
	if (current_pid == PID_MAX) current_pid = 2;
	else current_pid++;
	
	return current_pid;
}

void inner_task_switch(union task_union *t) {
	// Cambiamos el directorio actual
	set_cr3(get_DIR(&(t->task)));
	
	// Guardamos ebp en el pcb.
	__asm__ __volatile__(
		"movl %%ebp, %0"
		: "=m" (current()->esp)
		: // No input
	);
	
	// Movemos a esp el puntero de t.
	__asm__ __volatile__(
		"movl %0, %%ebp"
		: // No output
		: "r" (t->task.esp)
	);
	
	// Actualizamos tss
	tss.esp0 = KERNEL_ESP(t);
	
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}


/* SCHEDULING POLICY FUNCTIONS */

void sched_next_rr() {
	struct task_struct *next;
	
	if (current() != idle_task) update_process_state_rr(current(), &readyqueue);
	
	if (list_empty(&readyqueue)) next = idle_task;
	else {
		next = list_head_to_task_struct(list_first(&readyqueue));
		update_process_state_rr(next, NULL);
	}
	
	current_ticks_left = get_quantum(next);
	
	task_switch(next);
}

void update_process_state_rr(struct task_struct *t, struct list_head *dest) {
	if (t != current()) list_del(&(t->list));
	if (dest != NULL) list_add_tail(&(t->list), dest);
}

int needs_sched_rr() {
	// Si no queda quantum y no hay procesos esperando.
	if (current_ticks_left <= 0) {
		if (!list_empty(&readyqueue)) return 1;
		else current_ticks_left = 1; // No hay procesos en cola, le damos un tick más.
	}
	return 0;
}

void update_sched_data_rr() {
	current_ticks_left--;
}

void schedule() {
	update_sched_data_rr();
	if (needs_sched_rr()) sched_next_rr();
}

int get_quantum(struct task_struct *t) {
	return t->quantum;
}

void set_quantum(struct task_struct *t, int new_quantum) {
	t->quantum = new_quantum;
}
