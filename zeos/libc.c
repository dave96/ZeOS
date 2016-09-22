/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

int errno;

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

// Wrappers de llamadas en C.
// Aquí solo están los wrappers sencillos. Los que aceptan parámetros
// están implementados en assembler en libc_asm.S

int gettime() {
	int ret;
	__asm__ __volatile__ ("movl $10, %%eax\n\t"
						"int $0x80\n\t"
						"movl %%eax, %0"
						: "=r" (ret)
						:
						: "eax");	
	return ret;
}



