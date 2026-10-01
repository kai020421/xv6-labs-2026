#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int is_digit(char c){
return c>= '0' && c<= '9';
}

void process_fd(int fd){
char buf[512];
int n;
long current_num = 0;
int in_number = 0;

while ((n=read(fd, buf, sizeof(buf))) > 0) {
for(int i=0; i<n; i++){
char c= buf[i];
if (is_digit(c)){
current_num = current_num * 10 + (c- '0');
in_number = 1;
}
else{
if(in_number){
if(current_num % 5 == 0 || current_num % 6 == 0){
printf("%d\n", (int)current_num);
}
current_num = 0;
in_number = 0;
}}}}

if(in_number){
if(current_num % 5 ==0 || current_num % 6 == 0){
printf("%d\n", (int)current_num);
}}}

int main (int argc, char *argv[]){
if(argc<=1){
process_fd(0);
}
else{
for (int i=1; i<argc; i++){
int fd = open(argv[i], 0);
if(fd<0){
fprintf(2, "sixfive: cannot open %s\n", argv[i]);
continue;
}
process_fd(fd);
close(fd);
}
}
exit(0);
}
