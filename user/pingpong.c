#include "kernel/types.h"
#include "user/user.h"

  int main(){
    int p[2];
    char buf[100];
    if (pipe(p) < 0){
      printf( "error with creating pipe");
      exit(-1);
    }
    int pid=fork();
    if(pid ==0){
      write(p[1], "ping", 4);
      printf("%d: recieved ping\n", getpid());
    }
    else{
      wait(0);
      read(p[0], buf, 4);
      printf("%d: recieved pong", getpid());
    }
    exit(0);
  }


