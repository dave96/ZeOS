#include <libc.h>
#include <stats.h>

char buff[200];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
	/*int pid = clone(print_something, buff);
	
	if (pid < 0) perror();
	else pprint("Éxito!\n");
	while(1);
	return 0;
	* */

	while(read(0, buff, 1))
		write(1, buff, 1);
	
	while(1);
}




