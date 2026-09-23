#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 4){
    fprintf(2, "Usage: sandbox mask path/dash command [args...]\n");
    exit(1);
  }

  uint64 mask = atoi(argv[1]);
  char *path = 0;

  if(strcmp(argv[2], "-") != 0){
    path = argv[2];
  }

  if(interpose(mask, path) < 0){
    fprintf(2, "sandbox: interpose failed\n");
    exit(1);
  }

  exec(argv[3], &argv[3]);

  fprintf(2, "sandbox: exec %s failed\n", argv[3]);
  exit(0);
}
