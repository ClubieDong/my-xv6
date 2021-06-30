
#include "types.h"
#include "user.h"

int fib(int n)
{
  if (n <= 2)
    return 1;
  return fib(n - 1) + fib(n - 2);
}

int data[4096];//4 pages; cannot put it into stack 
int
main(int argc, char *argv[]){       
    int pre,post;
    int n,pid;    
  
    pre=get_free_frame_cnt();
    for(n=0;n<64;n++){
      pid=fork();
      if(pid==0){
        data[0]='a';        
        exit();
      }else if(pid<0)
        break;
    }
    printf(1,"created %d child processes\n",n);
    printf(1, "waiting, fib(25)=%d\n", fib(25));
    post=get_free_frame_cnt();
    while(n--)wait();
    printf(1,"pre: %d, post: %d\n",pre,post);    
    exit();
}

