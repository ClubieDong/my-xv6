#include "types.h"
#include "user.h"

extern int set_priority(int, int);
extern void enable_sched_display(int);

int fib(int i){
 if(i<=1)return i;
 return fib(i-1)+fib(i-2);
}
void busy_computing(){
   fib(30);
}


int main(){

        int i,pid;
        int fd[7][2];
        char c;
        printf(1,"================================\n");
        pid=getpid();
        set_priority(pid,1);
        printf(1,"Parent (pid=%d, prior=%d)\n",pid,1);
        for(i=0;i<6;i++){
                pipe(fd[i]);
                if((pid=fork())==0){
                        //child
                        close(fd[i][1]);//close write end
                        read(fd[i][0],&c,1);
                        busy_computing();
                        exit();
                }else{
                        close(fd[i][0]);//parent close read end
                        set_priority(pid,i%3+1);
                        printf(1,"Child (pid=%d, prior=%d) created!\n",pid,i%3+1);
                }
        }
        pipe(fd[6]);
        if((pid=fork())==0){//default priority
                //child
                close(fd[i][1]);//close write end
                busy_computing();
                exit();
        }else{
                close(fd[i][0]);//close read end
                printf(1,"Child (pid=%d, prior=%d) created!\n",pid,2);
        }
        printf(1,"================================\n");
        enable_sched_display(1);
        //parent wake up all children
        for(i=0;i<7;i++){
        c='a';
        write(fd[i][1],&c,1);
        close(fd[i][1]);
        }


        for(i=0;i<7;i++)
                wait();
        enable_sched_display(0);
        printf(1,"\n");
        exit();
        return 0;
}

