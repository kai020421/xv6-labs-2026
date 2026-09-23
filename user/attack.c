#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[]) {
    uint64 mask = 1;
    char *path = "/README";

    int ret = interpose(mask, path);
    printf("interpose returned: %d\n", ret);

    int fd = open("/README", O_RDONLY);
    printf("open returned fd: %d\n", fd);

    exit(0);
}
