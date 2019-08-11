#include "types.h"
#include "stat.h"
#include "user.h"

char buf[512];

void cat(int fd,int num_lines)
{
  int n;
  int line_count = 0;
  while((n = read(fd, buf, sizeof(buf))) > 0)
  {
    int buff_count = 0;
    int breakif = 0;
    while(buff_count <= n)
    {
      if(buf[buff_count] == '\n') line_count++;
      if(line_count == num_lines) 
      {
        breakif = 1;
        break;
      }
      int write_val = write(1, buf+buff_count, 1);
      if(write_val != 1) 
      {
        printf(1, "cat: write error\n");
        exit();
      } 
      buff_count++;
    }
    if(n < 0)
    {
      printf(1, "cat: read error\n");
      exit();
    }
    if(breakif == 1) break;
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    cat(0,0);
    exit();
  }

  int num_lines = atoi(argv[1]);

  for(i = 2; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      printf(1, "cat: cannot open %s\n", argv[i]);
      exit();
    }
    printf(1,"--- %s ---\n", argv[i]);
    cat(fd,num_lines);
    printf(1,"\n");
    close(fd);
  }
  exit();
}