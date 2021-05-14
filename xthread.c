#include "types.h"
#include "user.h"

// As defined in mmu.h
#define PGSIZE 4096
#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))

extern int clone(void *(*fn)(void *), void *stack, void *arg);
extern void join(int tid, void **ret_p, void **stack);
extern void thread_exit(void *ret);

int xthread_create(int *tid, void *(*start_routine)(void *), void *arg)
{
    // There must be a whole page in 2*PGSIZE
    void *p = malloc(PGSIZE * 2);
    if (!p)
        return -1;
    // Stack need to be aligned to page
    void *stack = (void*)PGROUNDUP((uint)p + sizeof(void *));
    // Save `p` just before stack, it's guaranteed that stack-4 >= p
    *((void **)stack-1) = p;
    int clone_res = clone(start_routine, stack, arg);
    if (clone_res == -1)
    {
        free(stack);
        return -1;
    }
    *tid = clone_res;
    return 1;
}

void xthread_exit(void *ret_val_p)
{
    thread_exit(ret_val_p);
}

void xthread_join(int tid, void **retval)
{
    void *stack;
    join(tid, retval, &stack);
    // The first address of 2*PGSIZE memory block is saved just before stack
    free(*((void**)stack-1));
}
