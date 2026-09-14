#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void memdump(char *fmt, char *data, int len) {
  for (int i = 0; fmt[i] != '\0'; i++) {
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
      printf("%%d\n", val);
      data += 4;
      len -= 4;
      break;
    }
    case 'p': {
      uint64 val;
      memmove(&val, data, 8);
      printf("%%p\n", (void *)val);
      data += 8;
      len -= 8;
      break;
    }
    case 'h': {
      short val;
      memmove(&val, data, 2);
      printf("%%d\n", val);
      data += 2;
      len -= 2;
      break;
    }
    case 'c': {
      printf("%%c\n", *data);
      data += 1;
      len -= 1;
      break;
    }
    case 's': {
      char *str_ptr;
      memmove(&str_ptr, data, 8);
      printf("%%s\n", str_ptr);
      data += 8;
      len -= 8;
      break;
    }
    case 'S': {
      for (int k = 0; k < len; k++) {
        if (data[k] == '\0') break;
        printf("%%c", data[k]);
      }
      printf("\n");
      return;
    }
    default:
      break;
    }
  }
  return;

not_enough:
  printf("memdump: not enough data for '%%c'\n", fmt[i]);
}
