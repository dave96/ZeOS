#ifndef __DATA_PORT
#define __DATA_PORT 0x60
#endif

#define LECTURA 0
#define ESCRIPTURA 1

// Error numbers
#define	EINVAL		22	/* Invalid argument */

// Estructuras
extern char char_map[];

// Interrupciones
void keyboard_routine();
void clock_routine();

// Llamadas a sistema
int sys_write(int fd, char * buffer, int size);

// Uso externo
void init_interrupt_handlers();

// Uso interno
