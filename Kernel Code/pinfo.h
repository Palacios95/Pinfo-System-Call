#include <linux/types.h>
struct pinfo {
 pid_t pid;                 /* process id */
 long state;                /* current state of process */
 long nice;                 /* process nice value */
 pid_t parent_pid;          /* process id of parent */
 int nr_children;           /* total number of child processes */
 int nr_threads;            /* total number of child threads */
 pid_t youngest_child_pid;  /* pid of youngest child */
 pid_t younger_sibling_pid; /* pid of younger sibling */
 pid_t older_sibling_pid;   /* pid of older sibling */
 unsigned long start_time;  /* process start time */
 long user_time;            /* CPU time spent in user mode */
 long sys_time;             /* CPU time spent in system mode */
 long cutime;               /* total user time of children */
 long cstime;               /* total system time of children */
 long uid;                  /* user id of process owner */
 char comm[16];             /* name of program executed */
};
