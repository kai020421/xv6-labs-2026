#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int 
main(void)
{
int free_b=freemem();
printf("Free memory: %d bytes (%d MB)\n", free_b, free_b/(1024 * 1024));
exit(0);
}
