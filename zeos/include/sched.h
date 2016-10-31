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

#define STATUS_DEAD 	0
#define STATUS_ALIVE	1

enum state_t { ST_RUN, ST_READY, ST_BLOCKED };

int current_pid;

int get_new_pid(void);

struct task_struct {
  int PID;			/* Process ID. This MUST be the first field of the struct. */
  page_table_entry * dir_pages_baseAddr;
  struct list_head list;
  unsigned long esp;
  int quantum;
  int status;
  struct stats st;
};


union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

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


#define KERNEL_ESP(t)       	(DWord) &(t)->stack[KERNEL_STACK_SIZE]

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

int current_ticks_left;

/* STATS */

void init_stats(struct task_struct *t);
inline void stats_enter_kernel();
inline void stats_exit_kernel();
inline void stats_enter_ready();
inline void stats_exit_ready(struct task_struct *t);
struct stats * get_stats_pid(int pid);

#endif  /* __SCHED_H__ */
