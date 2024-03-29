/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <types.h>
#include <mm_address.h>
#include <stats.h>

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024
#define PID_MAX 	32767
#define DEFAULT_QUANTUM  20
#define SEM_MAX_NUM		20

#define STATUS_DEAD 	0
#define STATUS_ALIVE	1

#define CEILING(X, A) (X%A > 0 ? X/A+1 : X/A)
#define TOTAL_HEAP_SIZE (current()->program_break-(PAG_LOG_INIT_HEAP*PAGE_SIZE))
#define LAST_HEAP_PAGE  PAG_LOG_INIT_HEAP+CEILING((current()->program_break-(PAG_LOG_INIT_HEAP*PAGE_SIZE)),PAGE_SIZE)

enum state_t { ST_RUN, ST_READY, ST_BLOCKED };

int current_pid;

int dir_alloc[NR_TASKS];

int get_new_pid(void);

struct semaphore {
	struct task_struct * owner; /* Anchor to owner TASK_STRUCT base address */
	unsigned int counter;
	struct list_head blocked;
};

struct task_struct {
  int PID;			/* Process ID. This MUST be the first field of the struct. */
  page_table_entry * dir_pages_baseAddr; /* Page directory address */
  struct list_head list; /* List anchor for blocked, ready and free queues */
  unsigned long esp; /* Process kernel stack pointer to restore on context switch */
  struct stats st; /* Info for get_stats syscall */
  int quantum; /* Scheduling variable */
  int status; /* ALIVE/DEAD */
  int blocked_count; /* Keyboard Interrupt Pending bytes */
  void * program_break; /* HEAP Limit (Program Break) */
};


union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

/* This structure is used for sizeof() type macros and can be used to map process execution context in the kernel stack */
struct execution_context {
	DWord ebx;
	DWord ecx;
	DWord edx;
	DWord esi;
	DWord edi;
	DWord ebp;
	DWord eax;
	DWord ds;
	DWord es;
	DWord fs;
	DWord gs;
	DWord eip;
	DWord cs;
	DWord eflags;
	DWord esp;
	DWord ss;
};

extern union task_union protected_tasks[NR_TASKS+2];
extern union task_union *task; /* Vector de tasques */
struct task_struct *idle_task;

struct list_head freequeue;
struct list_head readyqueue;
struct list_head keyboardqueue;

struct semaphore sem_array[SEM_MAX_NUM];


#define KERNEL_ESP(t)       	(DWord) &(t)->stack[KERNEL_STACK_SIZE]
#define DIR_POS(t)				(int) ((((int) t->dir_pages_baseAddr) - ((int) &dir_pages[0])) / sizeof(dir_pages[0]))

#define INITIAL_ESP       	KERNEL_ESP(&task[1])

/* Inicialitza les dades del proces inicial */
void init_task1(void);

void init_idle(void);

void init_sched(void);

struct task_struct * current();

void inner_task_switch(union task_union*t);

struct task_struct *list_head_to_task_struct(struct list_head *l);

int allocate_DIR(struct task_struct *t);

int get_current_pid();

page_table_entry * get_PT (struct task_struct *t) ;

page_table_entry * get_DIR (struct task_struct *t) ;

/* Headers for the scheduling policy */

void sched_next_rr();
void update_process_state_rr(struct task_struct *t, struct list_head *dest);
int needs_sched_rr();
void update_sched_data_rr();

int get_quantum(struct task_struct *t);
void set_quantum(struct task_struct *t, int new_quantum);

void schedule(void);

void block(struct task_struct *t);
void unblock(struct task_struct *t);

int current_ticks_left;

/* STATS */

void init_stats(struct task_struct *t);
void stats_enter_kernel();
void stats_exit_kernel();
void stats_enter_ready();
void stats_exit_ready(struct task_struct *t);
struct stats * get_stats_pid(int pid);

/* DIR manipulation */
void incr_DIR(struct task_struct *t);
void decr_DIR(struct task_struct *t);

#endif  /* __SCHED_H__ */
