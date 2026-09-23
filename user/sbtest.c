#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("Testing interpose system call...\n");
  
  // Test interpose call
  if(interpose(0, "") < 0){
    printf("interpose failed\n");
  } else {
    printf("interpose call success!\n");
  }

  exit(0);
}
