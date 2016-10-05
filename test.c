#include "toy_crt.h"

static const char *str = "Hello World!";

void test_puts()
{
    puts(str);
}

void test_iota() 
{
    int len = strlen(str);
    char len_str[10];
    itoa(len, len_str);
    puts(len_str);
}

void test_malloc()
{
    int *p_int = (int *) malloc(sizeof(int));
    *p_int = 10;
    char len_str[10];
    itoa(*p_int, len_str);
    puts(len_str);
    free(p_int);
}

int main(int argc,char * argv[])
{
    test_puts();
    test_iota();
    test_malloc();
    puts("argc:");
    putchar(argc + '0');
    putchar('\n');
    puts("argv:");
    int i;
	for (i = 0; i < argc; i++) {
		puts(argv[i]);
	}
    getchar();
    return 42;
}

