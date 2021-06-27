
#include "types.h"
#include "user.h"


int
main(int argc, char *argv[])
{
     char c;
     while(1){
        read(0,&c,1);
        write(0,&c,1);
     }     
   

}

