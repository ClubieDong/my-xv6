#include "types.h"
#include "user.h"

#define TOTAL_TEST_TRIALS 10

extern int fork_winner(int);

void test(){
    int i = 0;   
    int ret = 0;
 for (i = 0; i < TOTAL_TEST_TRIALS; i++)
    {
        printf(1, "\nTrial %d: ", i);
        ret = fork();
        if (ret < 0)
        {
            printf(1, "fork() failed (%d)\n", ret);
            exit();
        }
        else if (ret == 0) // child
        {
            printf(1, " child! ");
            exit();
        }

        // parent
        printf(1, " parent! ");
        if (ret != wait())
        {
            printf(1, "wait() failed!\n");
        }
    }

    printf(1, "\n");
}

int
main(int argc, char *argv[])
{
    

    printf(1,"Fork test\nSet child as winner");

    fork_winner(1);  
    test();
    
    printf(1,"\nSet parent as winner");
    fork_winner(0);//the default
    test();
    
   
    exit();
}

