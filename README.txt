Name: E Vishal Goud
Roll Number: 2402MC01
Set Number: 2

Q1: Page Protection with mprotect() and munprotect()
- Added a page-range permission helper in kernel/vm.c; it validates alignment, length, process bounds, and every PTE before changing any page.
- Declared the helper in kernel/defs.h and added syscall handling in kernel/sysproc.c.
- Registered the calls in kernel/syscall.h and kernel/syscall.c, with user declarations and stubs in user/user.h and user/usys.S.
- Reloaded CR3 after changing page permissions so stale translations cannot retain old write access.
- Added prottest.c to check read-only access, a child write fault, restored write access, and invalid ranges.

Q2: Priority-Based CPU Scheduler
- Added a priority field to struct proc in proc.h and initialized every process to priority 10 in allocproc().
- Implemented set_priority() in proc.c with process-table locking, range validation, and old-priority return.
- Added the set_priority system-call dispatch and user-space declaration/stub.
- Changed scheduler() to select the first RUNNABLE process with the smallest priority number.
- Added schedtest.c to run CPU-bound children at priorities 5, 10, and 15 and verify their completion order.

Build and run:
make qemu-nox CPUS=1

The saved console logs are Q1_output.txt and Q2_output.txt.
