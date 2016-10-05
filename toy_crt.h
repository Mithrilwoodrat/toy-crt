#ifndef TOY_CRT_H
#define TOY_CRT_H

/*
 * from 程序员的自我修养 minicrt
 */
#ifndef NULL
#define NULL (0)
#endif


#define X86_64 1
typedef unsigned long size_t;

typedef int FILE;
#define EOF (-1)

#define stdin ((FILE *)0)
#define stdout ((FILE *)1)
#define stderr ((FILE *)2)

int crt_io_init(void);
int crt_heap_init(void);

//from unistd.h
int read(int fd, void *buffer, size_t size);
int write(int fd, const void *buffer, size_t size);
void puts(const char *str);
void putchar(char c);
int getchar(void);


void itoa(int n, char s[]);
size_t strlen(const char *str);

//from stdlib.h
void *malloc(size_t size);
void free(void *ptr);

#endif
