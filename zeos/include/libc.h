/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#define	EINVAL		22	/* Invalid argument */
#define	EFAULT		14	/* Bad address */
#define	ENOMEM		12	/* Out of memory */
#define	ESRCH		 3	/* No such process */

#include <stats.h>

int errno;

int write(int fd, char *buffer, int size);

void itoa(int a, char *b);

int strlen(char *a);

int getpid();

int fork();

void exit();

int gettime();

void perror();

void exit(void);

int get_stats(int pid, struct stats *t);

#endif  /* __LIBC_H__ */
