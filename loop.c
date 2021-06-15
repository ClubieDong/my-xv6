#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int i,j;
  i=fork();
  if(i==0){
      //child
      for(j=0;j<100000;j++)//do some computation
      {
          if(j%1000==0){
              printf(1," c ");
          }
      }
      exit();
  }else{//parent
       for(j=0;j<100000;j++)//do some computation
      {
          if(j%1000==0){
              printf(1," p ");
          }
      }
      wait();
  }
  exit();
}
