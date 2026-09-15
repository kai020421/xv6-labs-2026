#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void memdump(char *fmt, char *data, int len) {
  int i;
  for (i = 0; fmt[i] != '\0'; i++) {
    char spec = fmt[i];

    if (spec == 'i' && len < 4) goto not_enough;
    if (spec == 'p' && len < 8) goto not_enough;
    if (spec == 'h' && len < 2) goto not_enough;
    if (spec == 'c' && len < 1) goto not_enough;
    if (spec == 's' && len < 8) goto not_enough;

    switch (spec) {
    case 'i': {
      int val;
      memmove(&val, data, 4);
      printf("%d\n", val);
      data += 4;
      len -= 4;
      break;
    }
    case 'p': {
      uint64 val;
      memmove(&val, data, 8);
      printf("%lx\n", val);
      data += 8;
      len -= 8;
      break;
    }
    case 'h': {
      short val;
      memmove(&val, data, 2);
      printf("%d\n", val);
      data += 2;
      len -= 2;
      break;
    }
    case 'c': {
      printf("%c\n", *data);
      data += 1;
      len -= 1;
      break;
    }
    case 's': {
      char *str_ptr;
      memmove(&str_ptr, data, 8);
      printf("%s\n", str_ptr);
      data += 8;
      len -= 8;
      break;
    }
    case 'S': {
      while (len > 0 && *data != '\0') {
        printf("%c", *data);
        data++;
        len--;
      }
      if (len > 0 && *data == '\0') {
        data++;
        len--;
      }
      printf("\n");
      break;
    }
    default:
      break;
    }
  }
  return;

not_enough:
  printf("memdump: not enough data for '%c'\n", fmt[i]);
}

void test_examples() {
  struct {
    int a;
    short b;
  } s1 = {61810, 2026};

  printf("Example 1:\n");
  memdump("ih", (char *)&s1, sizeof(s1));

  char *s2 = "a string";
  printf("Example 2:\n");
  memdump("s", (char *)&s2, sizeof(s2));

  char s3[] = "another";
  printf("Example 3:\n");
  memdump("S", s3, sizeof(s3));

  struct {
    uint64 a;
    int b;
    short c;
    char d;
    char e[8];
  } s4 = {0xbd0, 1819438967, 100, 'z', "xyzzy"};

  printf("Example 4:\n");
  memdump("pihcS", (char *)&s4, sizeof(s4));

  char s5[] = "hello\0world";
  printf("Example 5:\n");
  memdump("Sccccc", s5, sizeof(s5));
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    test_examples();
    exit(0);
  }

  if (argc != 2) {
    printf("Usage: memdump format\n");
    exit(1);
  }

  static char buf[512];
  int len = 0;
  int n;

  while ((n = read(0, buf + len, sizeof(buf) - len)) > 0) {
    len += n;
  }

  memdump(argv[1], buf, len);
  exit(0);
}
