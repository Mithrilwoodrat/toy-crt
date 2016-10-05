#ifndef TOY_CRT_H
#define TOY_CRT_H

/*
 * from 程序员的自我修养 minicrt
 */

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
int getchar(void);
void putchar(char c);

void itoa(int n, char s[]);
size_t strlen(const char *str);

#define NULL 0

#endif
