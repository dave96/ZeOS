#include <libc.h>

char buff[24];

int pid;

/*
int add(int a, int b) {
	int ret;
	__asm__ __volatile__ ("leal (%1, %2, 1), %0"
			: "=r" (ret)
			: "r" (a), "r" (b)
			);
	return ret;
}

long inner(long count) {
	long i, suma;
	suma = 0;
	for(i = 0; i < count; ++i) suma = add(suma, i);
	return suma;
}

long outer(long count) {
	long i, acum;
	acum = 0;
	for (i = 0; i < count; ++i) {
		acum = add(acum, inner(i));
		if (i == 49) {
			i = 49;
		}
	}
	return acum;
}
*/

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
     runjp();
     itoa(getpid(), buff);
     int size = strlen(buff);
     
     while(1) {
		 
		 write(1, buff, size);
	}
     
    return 0;
}




