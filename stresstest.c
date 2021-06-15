
#include "types.h"
#include "user.h"

int data[4096];//4 pages; cannot put it into stack 
int
main(int argc, char *argv[]){       
    int pre,post;
    int n,i,pid;    
  
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
    while(i<0xfffffff)i++; //wait for some time
    post=get_free_frame_cnt();
    while(n--)wait();
    printf(1,"pre: %d, post: %d\n",pre,post);    
    exit();
}

