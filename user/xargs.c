#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_LEN 512

int
main(int argc, char *argv[])
{
  char buf[MAX_LEN];
  char *args[MAXARG];
  int i, n;

  if (argc < 2) {
    fprintf(2, "Usage: xargs <command> [args...]\n");
    exit(1);
  }

  for (i = 1; i < argc; i++) {
    args[i - 1] = argv[i];
  }
  int base_argc = argc - 1;

  int index = 0;
  char c;
  while ((n = read(0, &c, 1)) > 0) {
    if (c == '\n') {
      buf[index] = 0;
      args[base_argc] = buf;
      args[base_argc + 1] = 0;

      if (fork() == 0) {
        exec(args[0], args);
        fprintf(2, "xargs: exec %s failed\n", args[0]);
        exit(1);
      }
      wait(0);
      index = 0;
    } else if (index < MAX_LEN - 1) {
      buf[index++] = c;
    }
  }
  exit(0);
}
