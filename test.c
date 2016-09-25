#include "toy_crt.h"

static const char *str = "Hello World!\n";

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

int main(int argc,char * argv[])
{
    test_puts();
    test_iota();
    getchar();
    return 42;
}










