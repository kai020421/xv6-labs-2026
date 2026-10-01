#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#pragma GCC diagnostic ignored "-Winfinite-recursion"

void
sieve(int p[2])
{
  int prime;
  if (read(p[0], &prime, sizeof(prime)) != sizeof(prime)) {
    close(p[0]);
    exit(0);
  }
  printf("prime %d\n", prime);

  int p2[2];
  pipe(p2);
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "primes: fork failed\n");
    exit(1);
  }
  if (pid == 0) {
    close(p[0]);
    close(p2[1]);
    sieve(p2);
  } else {
    close(p2[0]);
    int n;
    while (read(p[0], &n, sizeof(n)) == sizeof(n)) {
      if (n % prime != 0) {
        write(p2[1], &n, sizeof(n));
      }
    }
    close(p[0]);
    close(p2[1]);
    wait(0);
    exit(0);
  }
}

int
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "primes: fork failed\n");
    exit(1);
  }
  if (pid == 0) {
    close(p[1]);
    sieve(p);
  } else {
    close(p[0]);
    for (int i = 2; i <= 35; i++) {
      write(p[1], &i, sizeof(i));
    }
    close(p[1]);
    wait(0);
    exit(0);
  }
  exit(0);
}
