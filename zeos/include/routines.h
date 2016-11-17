#ifndef __DATA_PORT
#define __DATA_PORT 0x60
#endif

#define LECTURA 0
#define ESCRIPTURA 1

// Error numbers
#define	EINVAL		22	/* Invalid argument */
#define	EFAULT		14	/* Bad address */
#define	ENOMEM		12	/* Out of memory */
#define	ESRCH		 3	/* No such process */
#define	EAGAIN		11	/* Try again */
#define	EBUSY		16	/* Device or resource busy */
#define	EPERM		 1	/* Operation not permitted */

// Estructuras
extern char char_map[];
int	zeos_ticks;

// Interrupciones
void keyboard_routine();
void clock_routine();

// Llamadas a sistema
int sys_write(int fd, char * buffer, int size);
int sys_getpid();
int sys_gettime();
int sys_fork();
void sys_exit(void);
int sys_clone (void (*function)(void), void *stack);
int sys_sem_init (int n_sem, unsigned int value);
int sys_sem_wait (int n_sem);
int sys_sem_signal (int n_sem);
int sys_sem_destroy (int n_sem);

// Uso externo
void init_interrupt_handlers();

// Uso interno
int check_fd(int fd, int persmissions);
