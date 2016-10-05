#include "toy_crt.h"

// type
#define HEAP_BLOCK_FREE  0xABABABABABABABAB // 8 bytes
#define HEAP_BLOCK_USED  0xCDCDCDCDCDCDCDCD

typedef struct _heap_header
{
    unsigned long long type;
    size_t size;
    struct _heap_header* next;
    struct _heap_header* prev;
}heap_header;

#define ADDR_ADD(a, o) (((char *)(a)) + o)
#define HEADER_SIZE (sizeof(heap_header))

heap_header *list_head = NULL;

void free(void *ptr) 
{
    heap_header *header = (heap_header *)ADDR_ADD(ptr, -HEADER_SIZE);
    if (header->type != HEAP_BLOCK_USED) {
        return;
    }

    header->type = HEAP_BLOCK_FREE;
    //merge
    if (header->prev != NULL && header->prev->type == HEAP_BLOCK_FREE) {
        header->prev->next = header->next;
        if (header->next != NULL)
            header->next->prev = header->prev;
        header->prev->size += header->size;

        header = header->prev;
    }
    
    if (header->next != NULL && header->next->type == HEAP_BLOCK_FREE) {
        header->size += header->next->size;
        header->next = header->next->next;
    }
}

void *malloc(size_t size) 
{
    heap_header *header;
    if (size == 0) 
        return NULL;

    header = list_head;
    // find a FREE MEM
    while(header != NULL) {
        if (header->type == HEAP_BLOCK_USED) {
            header = header->next;
            continue;
        }

        if ((header->size > size + HEADER_SIZE) &&
            (header->size <= size + HEADER_SIZE *2))
        {
            header->type = HEAP_BLOCK_USED;
        }

        if (header->size > size + HEADER_SIZE *2) {
            heap_header *next = (heap_header*)ADDR_ADD(header, size+HEADER_SIZE);
            next->prev = header;
            next->next = header->next;
            next->type = HEAP_BLOCK_FREE;
            next->size = header->size - (size- HEADER_SIZE);
            header->next = next;
            header->size = size + HEADER_SIZE;
            header->type = HEAP_BLOCK_USED;
            return ADDR_ADD(header, HEADER_SIZE);
        }
        header = header->next;
    }
    return NULL;
}

/*
 *C library/kernel differences
 *     The return value described above for brk() is the behavior provided by the glibc wrapper function for the Linux brk()  system  call.   (On
 *     most  other implementations, the return value from brk() is the same; this return value was also specified in SUSv2.)  However, the actual
 *     Linux system call returns the new program break on success.  On failure, the system call returns the current  break.   The  glibc  wrapper
 *     function does some work (i.e., checks whether the new break is less than addr) to provide the 0 and -1 return values described above.

 *     On  Linux,  sbrk() is implemented as a library function that uses the brk() system call, and does some internal bookkeeping so that it can
 *     return the old break value.
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
    heap_header *header = NULL;
    // 32 MB heap size
    size_t heap_size = 1024 * 1024 * 32;

    base = (void*) brk(0);
    void *end = ADDR_ADD(base, heap_size);
    end = (void *) brk(end);
    
    if (!end) {
        return 0;
    }
    header = (heap_header*) base;

    header->size = heap_size;
    header->type = HEAP_BLOCK_FREE;
    header->next = NULL;
    header->prev = NULL;

    list_head = header;
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
