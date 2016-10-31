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

// Uso externo
void init_interrupt_handlers();

// Uso interno
int check_fd(int fd, int persmissions);
