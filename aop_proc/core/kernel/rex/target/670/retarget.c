
// The RPM shall not make use of RVCT's semihosting.
//#pragma import(__use_no_semihosting)
//#pragma import(__use_no_semihosting_swi)

//#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#if defined __ARMCC_VERSION__
//#include <rt_sys.h>
//#include <rt_heap.h>
#elif defined __clang__
#include <malloc.h>
#include <string.h>
#endif /* __clang__ */


#include "comdef.h"
#include "coredump.h"
#include "image_layout.h"
#include "CoreVerify.h"
// #include "swevent.h"


#if defined __clang__
typedef int FILEHANDLE;
#endif /* __clang__ */


const char __stdin_name[] ="STDIN";
const char __stdout_name[]="STDOUT";
const char __stderr_name[]="STDERR";


#if defined  __ARMCC_VERSION__
char** stack_base_address = (char**);
char** stack_end_address = (char**)&rpm_image_section_array[RPM_STACK].section_base;

#elif defined __clang__
extern char __aop_heap_base[];
extern char __aop_heap_limit[];
extern char __reclaim_cram_pool_base[];
extern char __reclaim_cram_end[];
extern char __reclaim_dram_pool_base[];
extern char __reclaim_dram_end[];
#endif /* __clang__ */


#if defined __ARMCC_VERSION__
__asm void __user_setup_stackheap()
{
#if 0
  IMPORT ||Image$$STACK_END$$Base||

  ldr r0, ||Image$$STACK_END$$Base||
  mov sp, r0

  ldr r3, =stack_end_address
  ldr r3, [r3]
  ldr r3, [r3]

  ldr r2, =AOP_STACK_FILL_WORD

stack_fill_loop
  sub r0, r0, #0x4
  str r2, [r0]
  cmp r0, r3
  bne stack_fill_loop

  mov r0, #0
  mov r2, #0
#endif
  bx lr
  ALIGN
}

#elif defined __clang__
__attribute__((used))
static const unsigned STACK_FILL_WORD = AOP_STACK_FILL_WORD;

/*
 * NOTE: Even though the reset vector sets the SP to the specified stack address,
 * have to override this function since the default __user_setup_stackheap() is still called
 * by __main(), and it dereference the __main() address as stack address.
 * Since it's non-zero, it set the weird value to SP, and crashes.
 */
__asm void __user_setup_stackheap()
{
    asm(".extern __aop_stack_limit");
    asm(".extern __aop_stack_base");

    asm("ldr r0, =__aop_stack_limit");
    asm("mov sp, r0");

    asm("ldr r3, =__aop_stack_base");

    asm("ldr r2, =STACK_FILL_WORD");
    asm("ldr r2, [r2]");

    /* fill the stack with known word to check stack usage */
asm("stack_fill_loop:");
    asm("sub r0, r0, #0x4");
    asm("str r2, [r0]");
    asm("cmp r0, r3");
    asm("bne stack_fill_loop");

    asm("mov r0, #0");
    asm("mov r2, #0");

    asm("bx lr");
}

#endif /* __clang__ */


FILEHANDLE _sys_open(const char *name, int openmode)
{
  return -1;
}

FILEHANDLE _sys_close(FILEHANDLE fh)
{
  return -1;
}

int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned len, int mode)
{
  return -1;
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
{
  return -1;
}

int _sys_ensure(FILEHANDLE fh)
{
  return -1;
}

long _sys_flen(FILEHANDLE fh)
{
  return -1;
}

int _sys_seek(FILEHANDLE fh, long pos)
{
  return -1;
}

int _sys_istty(FILEHANDLE fh)
{
  return -1;
}

int _sys_tmpnam(char *name, int fileno, unsigned maxlength)
{
  return 0;
}

char *_sys_command_string(char *cmd, int len)
{
  return 0;
}

void _ttywrch(int ch)
{
}

void _sys_exit(int return_code)
{
    abort();
}

typedef struct aop_heap_section_s* aop_heap_section_t;

typedef struct aop_heap_section_s
{
    char *base;              // section base address
    char *ptr;               // current section address used by malloc. Must be only incremented.
    char *limit;             // section limit address
    aop_heap_section_t next; // next section
} aop_heap_section_s;

typedef struct __Heap_Descriptor
{
    aop_heap_section_t section_list;
    unsigned           free_heap_space;
    unsigned           free_count;
    bool               free_disabled;
} __Heap_Descriptor;

static __Heap_Descriptor theHeap;


void prevent_further_heap_frees(void)
{
    theHeap.free_disabled = true;
}

extern int __Heap_DescSize(int zero)
{
    return sizeof(__Heap_Descriptor);
}

extern void __Heap_Initialize(struct __Heap_Descriptor *h)
{
    aop_heap_add_section(__aop_heap_base, __aop_heap_limit);
}

extern void __Heap_Finalize(struct __Heap_Descriptor *h) {}
extern void __Heap_ProvideMemory(struct __Heap_Descriptor *h, void *mem, size_t size) {}

/*
 * Heap Block Structure:
 *
 * +---------------------+--------------+
 * | Block Size (4-byte) | Block Memory |
 * +---------------------+--------------+
 *
 */
extern void *__Heap_Alloc(struct __Heap_Descriptor *h, size_t size)
{
    INTLOCK();

    aop_heap_section_t section = theHeap.section_list;
    char *mem = NULL;

    size = (size + sizeof(unsigned) + 7) & ~7; // ARM docs say to 8-byte align allocated blocks.
                                               // 4-byte to store the size of memory block used by free()
    while(section)
    {
        if(section->ptr + size <= section->limit)
        {
            *(unsigned*)section->ptr = size;
            mem = section->ptr + sizeof(unsigned);
            section->ptr += size;
            theHeap.free_heap_space -= size;
            break;
        }

        section = section->next;
    }

    if(!mem)
    {
        // Don't abort and let the caller handle malloc failure.

        //SWEVENT(RPM_OUT_OF_HEAP);
    }

    INTFREE();

    return mem;
}

/*
 * Heap Section Structure:
 *
 * +---------------+--------------+----------------+---------------+
 * | base (4-byte) | ptr (4-byte) | limit (4-byte) | next (4-byte) |
 * +---------------+--------------+----------------+---------------+
 * |                                                               |
 * | Heap Blocks / Heap Sections (Freed Heap Blocks)               |
 * |                                                               |
 * +---------------+--------------+----------------+---------------+
 *
 */
void aop_heap_add_section(char* base, char* limit)
{
    INTLOCK();

    aop_heap_section_t new_section = (aop_heap_section_t)base; //Use the beginning of the new section for section info.

    if(!base || !limit)
    {
        ERR_FATAL("aop_heap_add_section: invalid heap base/limit", 0, 0, 0);
    }

    // Need to be 4-byte aligned but not 8-byte aligned since 4-byte is used for block size before block memory and
    // block memory address need to be 8-byte aligned.
    base = (char*)(((unsigned)base + sizeof(aop_heap_section_s) + 3) & ~3);
    if(((unsigned)base & sizeof(unsigned)) == 0)
    {
        base += sizeof(unsigned);
    }

    limit = (char*)((unsigned)limit & ~7);

    if(base >= limit)
    {
        ERR_FATAL("aop_heap_add_section: invalid heap range", 0, 0, 0);
    }

    //zero fill it so that no one can accidentally read data from it that will be eventually overwritten by the heap.
    memset(base, 0, limit - base);

    new_section->base  = new_section->ptr = base;
    new_section->limit = limit;

    //Insert new section at the head
    new_section->next    = theHeap.section_list;
    theHeap.section_list = new_section;

    theHeap.free_heap_space += (limit - base);

    INTFREE();
}

static int aop_heap_is_heap_address(char* addr)
{
    aop_heap_section_t section = theHeap.section_list;
    int ret = 0;

    while(section)
    {
        if(section->base <= addr && addr < section->limit)
        {
            ret = 1;
            break;
        }

        section = section->next;
    }

    return ret;
}

// section header + 4-byte for block size + minimum block size
#define MINIMUM_HEAP_SECTION_SIZE   (sizeof(aop_heap_section_s) + sizeof(unsigned) + 4)

/*
 * Insert the freed heap block to the head of the section list
 * if the heap block size >= MINIMUM_HEAP_SECTION_SIZE
 */
extern void __Heap_Free(struct __Heap_Descriptor *h, void *ptr)
{
    char *base;
    unsigned size;

    // On the RPM we don't allow people to repeatedly alloc and free. That pattern leads to
    // heap fragmentation which will eventually cause the RPM to fail with OOM.
    // Better to just prevent the pattern in the first place.
    if(theHeap.free_disabled || !aop_heap_is_heap_address(ptr))
    {
        ERR_FATAL("__Heap_Free: free is disabled or invalid heap address", 0, 0, 0);
    }

    theHeap.free_count++;

    // block size is stored 4-byte before block address
    base = ptr - sizeof(unsigned);
    size = *(unsigned*)base;

    if(size >= MINIMUM_HEAP_SECTION_SIZE)
    {
        aop_heap_add_section(base, base + size);
    }
}

extern void *__Heap_Realloc(struct __Heap_Descriptor *h, void *old_mem, size_t size)
{
    void *new_mem = __Heap_Alloc(h, size);
    CORE_VERIFY(new_mem);
    memcpy(new_mem, old_mem, size);
    return new_mem;
}

extern void *__Heap_Stats(struct __Heap_Descriptor *h,
                          int (*print)(void *, char const *format, ...),
                          void *printparam)
{
    return NULL;
}

extern int __Heap_Valid(struct __Heap_Descriptor *h,
                        int (*print)(void *, char const *format, ...),
                        void *printparam, int verbose)
{
    return 1;
}

#if defined __clang__
/*
 * "--wrap" linker options are defined to use our own malloc/free/etc:
 *
 * aop_proc\tools\build\scons\scripts\sd_llvm.py
 */
void * __wrap_malloc(size_t size)
{
    return __Heap_Alloc(&theHeap, size);
}

void  __wrap_free(void* ptr)
{
    __Heap_Free(&theHeap, ptr);
}

void * __wrap_realloc(void *old_mem, size_t size)
{
    return __Heap_Realloc(&theHeap, old_mem, size);
}

void * __wrap_calloc(size_t m, size_t n)
{
    void * p = __Heap_Alloc(&theHeap, m*n);

    if (p)
    {
        size_t *z;

        n = (n + sizeof(*z) - 1)/sizeof(*z);

        for(z = p; n; n--,z++)
            *z = 0;
    }

    return p;
}

void aop_reclaim_code_ram_pool(void)
{
  aop_heap_add_section(__reclaim_cram_pool_base, __reclaim_cram_end);
  aop_heap_add_section(__reclaim_dram_pool_base, __reclaim_dram_end);
}

#endif /* __clang__ */

