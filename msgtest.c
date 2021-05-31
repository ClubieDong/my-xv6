#include "types.h"
#include "stat.h"
#include "user.h"

//you need to implement these system calls first
extern int msg_send(int,int,int,int);
extern int msg_receive(int*,int *, int *);
extern int alloc_sem(int);
extern int wait_sem(int);
extern int signal_sem(int);
extern int dealloc_sem(int);

//this line should not in any diff file 20190419

int
main(int argc, char *argv[]){
	
	
	int i,j;
	int a,b,c;

	//test whether resources are released at exit
	int child[20];
	printf(1,"\nchild(sender,child+3):  ");
	for(i=1;i<=120;i++){		
		if((j=fork())==0){
			//child, receive message			
			if((j=msg_receive(&a,&b,&c))>0){
				printf(1,"%d(%d,%d) ",getpid(),j,c);
			}
			exit();
		}else{//parent, record child info
			child[i%20]=j;			
		}
		if(i%20==0){
			for(j=0;j<20;j++){//parent sending messages
				msg_send(child[j%20],child[j%20]+1,child[j%20]+2,child[j%20]+3);					
			}
			for(j=0;j<20;j++)
				wait();
		}
	}
	printf(1,"\n");

	//test multiple senders 
	int s=alloc_sem(0),pid=getpid();
	for(i=0;i<20;i++){
		if(fork()==0){
			wait_sem(s);
			while(i<0xfffff)i++;//busy computing
			if(msg_send(pid,getpid(),0,0)<0){
				printf(1,"send msg failed from pid=%d\n",getpid());				
			}
			exit();
		}
	}
	for(i=0;i<20;i++)
		signal_sem(s);
	printf(1,"messages received from :",pid);
	for(i=0;i<20;i++){
		if(msg_receive(&a,&b,&c)<0){
			printf(1,"error receiving a message in parent\n");
		}else{
			printf(1,"%d ",a);
		}
	}
	printf(1,"\n");

	for(i=0;i<20;i++)
		wait();

	//test long message transmission
	pid=getpid();
	if((j=fork())==0){
		//child is the sender
		char *m="Hello World! Good Morning!";
		i=0;
		while(i<strlen(m)){
			//send one letter at a time
			msg_send(pid,(int)m[i],0,0);
			sleep(1);
			i++;
		}
		msg_send(pid,-1,0,0);
		exit();
	}else{
		//parent is the server
		do{
			msg_receive(&a,&b,&c);
			printf(1,"%c",(char)a);
		}while(a!=-1);		
	}
	wait();

	exit();
}