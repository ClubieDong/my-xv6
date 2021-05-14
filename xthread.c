#include "types.h"
#include "user.h"

extern int clone(void *(*fn)(void *),void *stack,void *arg);
extern void join(int tid,void **ret_p,void **stack);
extern void thread_exit(void *ret);


int xthread_create(int * tid, void * (* start_routine)(void *), void * arg)
{
    // add your implementation here ...
    return 0;
   
}


void xthread_exit(void * ret_val_p)
{
    // add your implementation here ...
   
    
}


void xthread_join(int tid, void ** retval)
{
    // add your implementation here ...
   
    
}
