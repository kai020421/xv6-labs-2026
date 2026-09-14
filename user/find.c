#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int has_exec = 0;
char *exec_argv[MAXARG];
int exec_argc = 0;

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}

void
find(char *path, char *filename)
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

  if(strcmp(fmtname(path), filename) == 0){
    if(has_exec){
      int pid = fork();
      if(pid < 0){
        fprintf(2, "find: fork failed\n");
        close(fd);
        return;
      }
      if(pid == 0){
        char *args[MAXARG];
        int i;
        for(i = 0; i < exec_argc; i++){
          args[i] = exec_argv[i];
        }
        args[exec_argc] = path;
        args[exec_argc + 1] = 0;
        
        exec(args[0], args);
        fprintf(2, "find: exec %s failed\n", args[0]);
        exit(1);
      } else {
        wait(0);
      }
    } else {
      printf("%s\n", path);
    }
  }

  switch(st.type){
  case T_FILE:
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      
      find(buf, filename);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "Usage: find <path> <filename> [-exec <cmd...>]\n");
    exit(1);
  }

  if(argc > 3){
    if(strcmp(argv[3], "-exec") != 0){
      fprintf(2, "find: unknown option %s\n", argv[3]);
      exit(1);
    }
    has_exec = 1;
    for(int i = 4; i < argc; i++){
      if(exec_argc >= MAXARG - 2){
        fprintf(2, "find: too many arguments for -exec\n");
        exit(1);
      }
      exec_argv[exec_argc++] = argv[i];
    }
  }

  find(argv[1], argv[2]);
  int ticks = atoi(argv[1]);
  pause(ticks);
  exit(0);
};
}
