#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data)
{

  /*
  This exercise will give you more practice using C pointers. Before starting read section 5.1 (Pointers and addresses) through 5.6 (Pointer arrays) and 6.4 (pointers to structures) in "The C programming language (second edition)" by Kernighan and Ritchie (K&R).
Have a look at user/memdump.c. Your job is to implement the function memdump(char *fmt, char *data). memdump()'s purpose is to print the contents of the memory pointed to by data in the format described by the fmt argument. The format is a C string. Each character of the string indicates how to print successive parts of the data. Thus, for example, a C struct with multiple fields can be printed with a format string containing multiple characters.

Your memdump() should handle the following format characters:

    i: print the next 4 bytes of the data as a 32-bit integer, in decimal.
    p: print the next 8 bytes of the data as a 64-bit integer, in hex.
    h: print the next 2 bytes of the data as a 16-bit integer, in decimal.
    c: print the next 1 byte of the data as an 8-bit ASCII character.
    s: the next 8 bytes of the data contain a 64-bit pointer to a C string; print the string.
    S: the rest of the data contains the bytes of a null-terminated C string; print the string. 
  */
  char *temp=fmt;

  while(*temp!='\0')
  {
    if(*temp=='i')
    {
      printf("%d\n", *(int *)data);
      data+=4;
      
    }
    if(*temp=='p')
    {
      printf("%lx\n", *(unsigned long *)data);
      data+=8;
      
    }
    if(*temp=='h')
    {
      printf("%d\n", *(short *)data);
      data+=2;
      
    }
    if(*temp=='c')
    {
      printf("%c\n", *data);
      data+=1;
      
    }
    if(*temp=='s')
    {
      printf("%s\n", *(char **)data);
      data+=8;
      
    }
    if(*temp=='S')
    {
      printf("%s\n", data);
      
    }
    temp++;

  }

}
