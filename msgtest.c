#include "types.h"
#include "stat.h"
#include "user.h"

extern int msg_send(int, int, int, int);
extern int msg_receive(int *, int *, int *);
extern int alloc_sem(int);
extern int wait_sem(int);
extern int signal_sem(int);
extern int dealloc_sem(int);

int main()
{
	alloc_sem(1);
	wait_sem(2);
	signal_sem(3);
	dealloc_sem(4);
	msg_send(1, 2, 3, 4);
	msg_receive((void *)0, (void *)1, (void *)2);
	exit();
}
