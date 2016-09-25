#include "toy_crt.h"

#define ADDR_ADD(a, o) (((char *)(a)) + o)
#define HEADER_SIZE (sizeof(heap_header))

void *malloc(size_t size);
void free(void *ptr);

/*
    C library/kernel differences
       The return value described above for brk() is the behavior provided by the glibc wrapper function for the Linux brk()  system  call.   (On
       most  other implementations, the return value from brk() is the same; this return value was also specified in SUSv2.)  However, the actual
       Linux system call returns the new program break on success.  On failure, the system call returns the current  break.   The  glibc  wrapper
       function does some work (i.e., checks whether the new break is less than addr) to provide the 0 and -1 return values described above.

       On  Linux,  sbrk() is implemented as a library function that uses the brk() system call, and does some internal bookkeeping so that it can
       return the old break value.
 */
static long brk(void *addr) 
{
    long ret = 0;
    __asm__ volatile(
        "movq $12, %%rax \n\t"
        "movq %1, %%rdi \n\t"
        "syscall \n\t"
        "movq %%rax, %0 \n\t"
        : "=r" (ret)
        : "m" (addr)
        );
}

int crt_heap_init()
{
    void *base = NULL;
    //heap_header *header = NULL;
    // 32 MB heap size
    size_t heap_size = 1024 * 1024 * 32;

    base = (void*) brk(0);
    void *end = ADDR_ADD(base, heap_size);
    end = (void *) brk(end);
    
    if (!end) {
        return 0;
    }
    
    return 1;
}
/*
  after crt_heap_init
  cat /proc/4680/maps                                                         
  00400000-00401000 r-xp 00000000 08:01 29099192                           /home/woodrat/Desktop/toy-crt/bin/test
  00601000-00602000 rw-p 00001000 08:01 29099192                           /home/woodrat/Desktop/toy-crt/bin/test
  01643000-03643000 rw-p 00000000 00:00 0                                  [heap]
  7ffcf89ce000-7ffcf89ef000 rw-p 00000000 00:00 0                          [stack]
  7ffcf89f5000-7ffcf89f7000 r--p 00000000 00:00 0                          [vvar]
  7ffcf89f7000-7ffcf89f9000 r-xp 00000000 00:00 0                          [vdso]
  ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0                  [vsyscall]

  heap field = 03643000 - 01643000  = 32M
 */
