#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int i, pid;
  int priorities[3] = {5, 10, 15};
  volatile uint count;
  int old_priority;

  old_priority = set_priority(getpid(), 1);
  printf(1, "parent priority set to 1 (old %d)\n", old_priority);

  for(i = 0; i < 3; i++){
    pid = fork();
    if(pid < 0){
      printf(1, "schedtest: fork failed\n");
      exit();
    }
    if(pid == 0){
      for(count = 0; count < 20000000; count++)
        ;
      printf(1, "priority %d finished\n", priorities[i]);
      exit();
    }
    old_priority = set_priority(pid, priorities[i]);
    if(old_priority < 0){
      printf(1, "schedtest: set_priority failed for pid %d\n", pid);
      exit();
    }
  }

  wait();
  wait();
  wait();
  printf(1, "Done\n");
  exit();
}
