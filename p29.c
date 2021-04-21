#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    printf(1, "OS Lab 161810129: ");
    if (argc == 1)
    {
        printf(1, "\n");
        exit();
    }
    for (int i = 1; i < argc; i++)
        printf(1, "%s%s", argv[i], i + 1 < argc ? " " : "\n");
    exit();
}
