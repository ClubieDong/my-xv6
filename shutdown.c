#include "types.h"
#include "stat.h"
#include "user.h"

extern int shutdown(int a);

void print_usage_and_exit()
{
  printf(1, "usage:\n    shutdown <code>    Shutdown with leave code\n");
  exit();
}

int
main(int argc, char *argv[])
{
  if (argc != 2)
    print_usage_and_exit();
  // Sadly `atoi` defined in `ulib.c` does not validate the input string,
  // I need to implement my own version.
  int a = 0;
  for (char *p = argv[1]; *p; ++p)
  {
    if (*p < '0' || *p > '9')
      print_usage_and_exit();
    a = a * 10 + (*p - '0');
  }
  shutdown(a);
  exit();
}
