#include "types.h"
#include "stat.h"
#include "user.h"

//you need to implement these system calls first
extern int alloc_sem(int);
extern int wait_sem(int);
extern int signal_sem(int);
extern int dealloc_sem(int);

void compute(int sem){//wait on a semaphore, print something, and then signal the semaphore
  int pid=getpid(),i=0;
  sleep(3);
  if((wait_sem(sem))<0){
    printf(1,"wait error, pid=%d, sem=%d\n",pid,sem);
    return;
  }  
  for(i=0;i<20;i++){
    printf(1,"-%d-",pid);
    sleep(3);
  }
  printf(1,"\nleaving(%d)\n",pid,sem);
  if(signal_sem(sem)<0){
    printf(1,"signal error, pid=%d, sem=%d\n",pid,sem);
    return;
  }
}

//20190419

int
main(int argc, char *argv[])
{

//illegal input
   printf(1,"illegal input test: ");
  int flag=0,i;
  if(alloc_sem(-1)>0||wait_sem(100)>0||signal_sem(100)>0||wait_sem(-1)>0||signal_sem(-1)>0||dealloc_sem(-1)>0){//should return -1
    flag=1;
  }
  for(i=0;i<100;i++){
    if(wait_sem(i)>0||signal_sem(i)>0||dealloc_sem(i)>0)
      flag=1;
  }
 
  if(flag==1){
    printf(1,"fail!\n");
  }else{
    printf(1,"success!\n");
  }

  flag=0;
  //robust
  printf(1,"alloc-dealloc test: ");
  for(i=0;i<100;i++){
    if(alloc_sem(100)<0)
      flag=1;
  }
  for(i=0;i<100;i++){
    if(dealloc_sem(i)<0)
      flag=1;
  }
  if((i=alloc_sem(100))<0||dealloc_sem(i)<0)
    flag=1;
  if(flag==1){
    printf(1,"fail!\n");
  }else{
    printf(1,"success!\n");
  }

   //create 3 semaphores with different initial values
   int s[3],j;
   for(i=0;i<3;i++){
     if((s[i]=alloc_sem(i+1))<0){
       printf(1,"error creating semaphore, please debug\n");
       return -1;
     }
   } 
   printf(1,"created semaphores with indices %d %d %d \n",s[0],s[1],s[2]);


  for(i=0;i<3;i++){//test the semaphores one by one
     printf(1,"testing semaphore %d with initial value %d\n",s[i],i+1);
     for(j=0;j<5;j++){//create 5 children
       if(fork()==0){//child
         compute(s[i]);
         exit();
       }
     }
    for(j=0;j<5;j++) wait();//wait for five children to die
  }
   
  printf(1,"normal test done\n");
  printf(1,"testing dealloc_sem\n");
  if(fork()==0){
    fork();
    fork();
     printf(1,"pid=%d is going to wait on sem %d\n",getpid(),s[0]);
     wait_sem(s[0]);
     printf(1,"pid=%d wait on sem %d: success\n",getpid(),s[0]);
     wait_sem(s[0]);
     printf(1,"should not get here\n");     
     exit();
  }
  sleep(100);
  dealloc_sem(s[0]);
  dealloc_sem(s[1]);
  dealloc_sem(s[2]);
  wait();
  printf(1,"parent leaving \n");
  exit(); 
}
