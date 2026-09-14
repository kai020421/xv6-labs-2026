#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int match(char*, char*);

char*
fmtname(char *path)
{
  char *p;
  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  return p;
}

void
run_exec(char *path, int argc, char *argv[])
{
  char *nargv[32];
  int i, n = 0;
  int used_placeholder = 0;

  if(argc > 30)
    argc = 30;

  for(i = 0; i < argc; i++) {
    if(strcmp(argv[i], "{}") == 0) {
      nargv[n++] = path;
      used_placeholder = 1;
    } else {
      nargv[n++] = argv[i];
    }
  }

  if(!used_placeholder) {
    nargv[n++] = path;
  }
  nargv[n] = 0;

  int pid = fork();
  if(pid < 0) {
    fprintf(2, "find: fork failed\n");
    return;
  }
  if(pid == 0) {
    exec(nargv[0], nargv);
    fprintf(2, "find: exec %s failed\n", nargv[0]);
    exit(1);
  }
  wait(0);
}

void
find(char *path, char *target, int exec_argc, char *exec_argv[])
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if(match(target, fmtname(path))){
      printf("%s\n", path);
      if(exec_argc > 0)
        run_exec(path, exec_argc, exec_argv);
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
      printf("find: path too long\n");
      close(fd);
      return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }

      if(strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
        continue;

      if(match(target, p)){
        printf("%s\n", buf);
        if(exec_argc > 0)
          run_exec(buf, exec_argc, exec_argv);
      }

      if(st.type == T_DIR){
        find(buf, target, exec_argc, exec_argv);
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "Usage: find <path> <expression> [-exec cmd ...]\n");
    exit(1);
  }

  int exec_idx = 0;
  for(int i = 3; i < argc; i++) {
    if(strcmp(argv[i], "-exec") == 0) {
      exec_idx = i + 1;
      break;
    }
  }

  if(exec_idx > 0 && exec_idx < argc) {
    int exec_argc = argc - exec_idx;
    if(exec_argc > 0 && (strcmp(argv[argc - 1], ";") == 0 || strcmp(argv[argc - 1], "\\;") == 0)) {
      exec_argc--;
    }
    find(argv[1], argv[2], exec_argc, &argv[exec_idx]);
  } else {
    find(argv[1], argv[2], 0, 0);
  }
  exit(0);
}

// --- Regular expression & exact matcher ---
int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(strcmp(re, text) == 0)
    return 1;
  if(re[0] == '^')
    return matchhere(re+1, text);
  return matchhere(re, text);
}

int
matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return *text == '\0';
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

int
matchstar(int c, char *re, char *text)
{
  do{
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}
