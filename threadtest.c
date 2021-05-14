#include "types.h"
#include "user.h"

extern int xthread_create(int *, void * (* )(void *), void * );
extern void xthread_exit(void * );
extern void xthread_join(int , void ** );


int count = 0;


void add_then_exit(int a, int b)
{
    int n = a + b;
    xthread_exit((void *)n);
}


/***************************** TEST Return Value *********************************
The expected output would look like (depending how the threads are scheduled, the outputs may be mixed up a little bit):

-------------------- Test Return Value --------------------
Child thread 1: count=3
Child thread 2: count=3
Main thread: thread 1 returned 2
Main thread: thread 2 returned 3

****************************************************************************/
void * tc_func1(void * arg)
{
   count++;
   sleep(100); // 1 second
   printf(1, "Child thread %d: count=%d\n", (int)arg, count);
   add_then_exit((int)arg,1);
   return (void *)0;
}

void * tc_func2(void *arg){
    count++;
    sleep(100);
    printf(1,"Child thread %d: count=%d\n",(int)arg,count);
    return (void *) ((int)arg)+1;
}

void tc1(void)
{
    int tid_1 = 0, tid_2 = 0, ret1,ret2;

    printf(1, "\n-------------------- Test Return Value --------------------\n");
    count = 0;
    xthread_create(&tid_1, tc_func1, (void *)1);
    xthread_create(&tid_2, tc_func2, (void *)2);
    count++;

    xthread_join(tid_1,(void **)&ret1);
    printf(1, "Main thread: thread 1 returned %d\n",ret1);


    xthread_join(tid_2,(void **)&ret2);

    printf(1, "Main thread: thread 2 returned %d\n",ret2);
}




/***************************** Test Stack space*********************************
The expected output would will like:

-------------------- Test Stack Space --------------------
ptr1 - ptr2 = 16
Return value 123

****************************************************************************/
void * tc2_func(void * arg)
{
    sleep(100); // 1 second    
    return (void *)0x123;
}

void tc2(void)
{
    int tid;
    void * ret_val;
    void * ptr1 = (void *)0, * ptr2 = (void *)0;


    printf(1, "\n-------------------- Test Stack Space --------------------\n");
    ptr1 = malloc(1);
    xthread_create(&tid, tc2_func, (void *)0);
    xthread_join(tid, &ret_val);
    ptr2 = malloc(1);
    printf(1, "ptr1 - ptr2 = %d\nReturn value %x\n", ptr1 - ptr2,ret_val);      

    if (ptr1)
        free(ptr1);
    if (ptr2)
        free(ptr2);    
}

/***************************** Test Thread Count*********************************
The expected output would will like (The exact number may be different, but the parent should create more threads):

-------------------- Test Thread Count --------------------
Child process created 60 threads
Parent process created 61 threads
****************************************************************************/
void * tfunc(void *arg){
    xthread_exit((void *)0);
    return (void *)0;
}

void tc3(void){
    //count the number of threads that can be created
    int count=0,tid;


    printf(1, "\n-------------------- Test Thread Count --------------------\n");
    if(fork()==0){
        //in child process       
        while(xthread_create(&tid,tfunc,(void *)0)>0){
            //success
            count++;
        }
            //cannot create any thread anymore, print and exit
        printf(1,"Child process created %d threads\n",count);
        exit();
        
    }    
    wait();
    while(xthread_create(&tid,tfunc,(void *)0)>0){
        count++;
    }
    printf(1,"Parent process created %d threads\n",count);
    
}

/************************** main() *****************************/
int main(int argc, char *argv[])
{ 
   
    tc1();
       
    tc2();
      
    tc3();      
      
    exit();
}


