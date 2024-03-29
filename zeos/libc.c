/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

// Wrappers de llamadas a sistema.

int gettime() {
	int ret;
	__asm__ __volatile__ ("movl $10, %%eax\n\t"
						"int $0x80\n\t"
						"movl %%eax, %0"
						: "=r" (ret)
						:
						: "eax");
	if (ret < 0) {
		errno = ret*(-1);
		return -1;
	}
	return ret;
}

int getpid() {
	int ret;
	__asm__ __volatile__ ("movl $20, %%eax\n\t"
						"int $0x80\n\t"
						"movl %%eax, %0"
						: "=r" (ret)
						:
						: "eax");
	if (ret < 0) {
		errno = ret*(-1);
		return -1;
	}
	return ret;
}

int fork() {
	int ret;
	__asm__ __volatile__ ("movl $2, %%eax\n\t"
						"int $0x80\n\t"
						"movl %%eax, %0"
						: "=r" (ret)
						:
						: "eax");
	if (ret < 0) {
		errno = ret*(-1);
		return -1;
	}
	return ret;
}

int write(int fd, char *buffer, int size) {
	int ret;
	
	__asm__ __volatile__ ("movl $4, %%eax\n\t"
						"int $0x80\n\t"
						"movl %%eax, %0"
						: "=r" (ret)
						: "b" (fd), "c" (buffer), "d" (size)
						: "eax");
	
	if (ret < 0) {
		errno = ret*(-1);
		return -1;
	}
	return ret;
}

int read(int fd, char *buf, int count) {
	int ret;
	
	__asm__ __volatile__ ("movl $3, %%eax\n\t"
						"int $0x80\n\t"
						"movl %%eax, %0"
						: "=r" (ret)
						: "b" (fd), "c" (buf), "d" (count)
						: "eax");
	
	if (ret < 0) {
		errno = ret*(-1);
		return -1;
	}
	return ret;
}

void pprint(char * s) {
	write(1, s, strlen(s));
}

void perror() {
	if (errno == EINVAL) pprint("Error: Invalid argument\n");
	else if (errno == EFAULT) pprint("Error: Invalid memory address\n");
	else if (errno == ENOMEM) pprint("Error: No memory availible\n");
	else if (errno == ESRCH) pprint("Error: No such process\n");
}

int get_stats(int pid, struct stats *t) {
	int ret;
	
	__asm__ __volatile__ ("movl $35, %%eax\n\t"
						"int $0x80\n\t"
						"movl %%eax, %0"
						: "=r" (ret)
						: "b" (pid), "c" (t)
						: "eax");
	
	if (ret < 0) {
		errno = ret*(-1);
		return -1;
	}
	return ret;
}

int sem_init (int n_sem, unsigned int value) {
	int ret;
	
	__asm__ __volatile__ ("movl $21, %%eax\n\t"
						"int $0x80\n\t"
						"movl %%eax, %0"
						: "=r" (ret)
						: "b" (n_sem), "c" (value)
						: "eax");
	
	if (ret < 0) {
		errno = ret*(-1);
		return -1;
	}
	return ret;
}

int sem_wait (int n_sem) {
	int ret;
	
	__asm__ __volatile__ ("movl $22, %%eax\n\t"
						"int $0x80\n\t"
						"movl %%eax, %0"
						: "=r" (ret)
						: "b" (n_sem)
						: "eax");
	
	if (ret < 0) {
		errno = ret*(-1);
		return -1;
	}
	return ret;
}

int sem_signal (int n_sem) {
	int ret;
	
	__asm__ __volatile__ ("movl $23, %%eax\n\t"
						"int $0x80\n\t"
						"movl %%eax, %0"
						: "=r" (ret)
						: "b" (n_sem)
						: "eax");
	
	if (ret < 0) {
		errno = ret*(-1);
		return -1;
	}
	return ret;
}

int sem_destroy (int n_sem) {
	int ret;
	
	__asm__ __volatile__ ("movl $24, %%eax\n\t"
						"int $0x80\n\t"
						"movl %%eax, %0"
						: "=r" (ret)
						: "b" (n_sem)
						: "eax");
	
	if (ret < 0) {
		errno = ret*(-1);
		return -1;
	}
	return ret;
}

void exit() {
	__asm__ __volatile__ ("movl $1, %%eax\n\t"
						"int $0x80"
						: // No output
						: // No input
						: "eax");
}

int clone (void (*function)(void), void *stack) {
	int ret;
	
	__asm__ __volatile__ ("movl $19, %%eax\n\t"
						"int $0x80\n\t"
						"movl %%eax, %0"
						: "=r" (ret)
						: "b" (function), "c" (stack)
						: "eax");
	
	if (ret < 0) {
		errno = ret*(-1);
		return -1;
	}
	return ret;
}

void * sbrk (int increment) {
	void * ret;
	
	__asm__ __volatile__ ("movl $45, %%eax\n\t"
						"int $0x80\n\t"
						"movl %%eax, %0"
						: "=r" (ret)
						: "b" (increment)
						: "eax");
	
	if (ret < 0) {
		errno = ((int) ret) * (-1);
		return -1;
	}
	return ret;
}

