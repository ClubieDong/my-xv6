#include "types.h"
#include "stat.h"
#include "user.h"

extern int shutdown();

int
main(int argc, char *argv[])
{
  shutdown();
  exit();
}
