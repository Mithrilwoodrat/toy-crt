#include "toy_crt.h"

extern int main(int argc, char** argv);
void _exit(int);

void die(char * msg) 
{
//    printf("die on : %s", msg);
    puts(msg);
    _exit(1);
}
/*
  in glibc /Sysdeps/X86_64/start.S
  This is the canonical entry point, usually the first thing in the text
   segment.  The SVR4/i386 ABI (pages 3-31, 3-32) says that when the entry
   point runs, most registers' values are unspecified, except for:

   %rdx		Contains a function pointer to be registered with `atexit'.
		This is how the dynamic linker arranges to have DT_FINI
		functions called for shared libraries that have been loaded
		before this code runs.

   %rsp		The stack contains the arguments and environment:
		0(%rsp)				argc
		LP_SIZE(%rsp)			argv[0]
		...
		(LP_SIZE*argc)(%rsp)		NULL
		(LP_SIZE*(argc+1))(%rsp)	envp[0]
		...
						NULL

    Extract the arguments as encoded on the stack and set up
	   the arguments for __libc_start_main (int (*main) (int, char **, char **),
		   int argc, char *argv,
		   void (*init) (void), void (*fini) (void),
		   void (*rtld_fini) (void), void *stack_end).
	   The arguments are passed via registers and on the stack:
	main:		%rdi
	argc:		%rsi
	argv:		%rdx
	init:		%rcx
	fini:		%r8
	rtld_fini:	%r9
	stack_end:	stack.	             
 */

static inline int syscall_1(long long syscallnum, long long arg0, long long arg1, long long arg2)
{
  register long long syscallnum_ __asm__("eax");
  register long long arg0_ __asm__("edi");
  register long long arg1_ __asm__("rsi");
  register long long arg2_ __asm__("edx");
  syscallnum_ = syscallnum;
  arg0_ = arg0;
  arg1_ = arg1;
  arg2_ = arg2;
  asm volatile
    (
        "syscall"
        : "+r"(syscallnum_)
        : "r"(arg0_), "r"(arg1_), "r"(arg2_)
        : "%rcx", "%r11", "memory"
    );
  return syscallnum_;
}

// never returns, when ends call syscall exit 
void crt_entry(void) 
{
    int ret;
    int argc;
    char** argv;

    char * ebp_reg = 0;

    // on 64bit system use rbp instead of ebp
    asm volatile("movq %%rbp, %0 \n":"=r" (ebp_reg)); 
    
    argc = *(int *) (ebp_reg + 8);
    argv = (char **) (ebp_reg + 16);

    
    if ( !crt_heap_init()){
        die("heap init failed!");
    }

    if ( !crt_io_init()) {
        die("IO init failed!");
    }

    ret = main(argc, argv);
    _exit(ret);
}

/*
  on x86_64 system, see csapp section 3
  rax - temporary register; when we call a syscal, rax must contain syscall number
  rdx - used to pass 3rd argument to functions
  rdi - used to pass 1st argument to functions
  rsi - pointer used to pass 2nd argument to functions
 */

//exit syscall is 60 on x86_64
void _exit(int status)
{
    
    __asm__("movq $60, %%rax \n\t"
            "movq %0, %%rdi \n\t"
            "syscall \n\t"
            "hlt \n\t"/* Crash if somehow `exit' does return.	 */
            :: "g" (status)); /* input */
}

