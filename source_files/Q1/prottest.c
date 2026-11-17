#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *page;
  int pid;

  page = sbrk(4096);
  if(page == (char*)-1){
    printf(1, "prottest: sbrk failed\n");
    exit();
  }
  *page = 'A';

  printf(1, "mprotect returned %d\n", mprotect(page, 1));
  printf(1, "read-only page contains %c\n", *page);
  pid = fork();
  if(pid < 0){
    printf(1, "prottest: fork failed\n");
    exit();
  }
  if(pid == 0){
    *page = 'B';
    printf(1, "ERROR: child write was not blocked\n");
    exit();
  }
  wait();
  printf(1, "parent still sees %c after child fault\n", *page);

  printf(1, "munprotect returned %d\n", munprotect(page, 1));
  *page = 'C';
  printf(1, "writable page now contains %c\n", *page);

  printf(1, "unaligned address: %d\n", mprotect(page + 1, 1));
  printf(1, "zero length: %d\n", mprotect(page, 0));
  printf(1, "range beyond process size: %d\n",
         mprotect(page + 4096, 1));
  exit();
}
