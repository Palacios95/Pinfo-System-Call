/*##########################################################
## COP4610 – Principles of Operating Systems – Summer C 2017
## Prof. Jose F. Osorio
## Student Name: Alejandro Palacios – 5156050
##
## Project: Customizing Linux Kernel
## Specs:   Adding Static System Call
## Due Date: 06/12/2017 by 11:55pm
## Module Name: test_pinfo.c
##
## I Certify that this program code has been written by me
## and no part of it has been taken from any sources.
##########################################################*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <omp.h>

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

//Calls the pinfo system call and prints out values of the pinfo struct passed into it.
void call_pinfo();
//Creates n child processes that call the pinfo syscall.
void create_processes(int num);
//Creates n threads.
void create_threads(int num);

//Main function.
int main()
{
	 create_processes(3);
	 create_threads(5);
	 call_pinfo();
         return 0;
}

void call_pinfo() 
{

	 struct pinfo p_info;
         long int return_val = syscall(301, &p_info);
	 printf("\n----------SYSCALL TO PINFO----------\n");
         printf("\tSystem call returned %ld \n", return_val);
	 printf("\tPID: %d\n", p_info.pid);
	 printf("\tSTATE: %ld\n", p_info.state);
	 printf("\tNICE: %ld\n", p_info.nice);
	 printf("\tPARENT PID: %d\n", p_info.parent_pid);
	 printf("\tNUM CHILDREN: %d\n", p_info.nr_children);
	 printf("\tNUM THREADS: %d\n", p_info.nr_threads);
         printf("\tYOUNGEST CHILD PID: %d\n", p_info.youngest_child_pid);
	 printf("\tYOUNGER SIBLING PID: %d\n", p_info.younger_sibling_pid);
         printf("\tOLDER SIBLING PID: %d\n", p_info.older_sibling_pid);
	 printf("\tSTART TIME: %ld\n", p_info.start_time);
	 printf("\tUSER TIME: %ld\n", p_info.user_time);
	 printf("\tSYSTEM TIME: %ld\n", p_info.sys_time);
	 printf("\tTOTAL CHILD USER TIME: %ld\n", p_info.cutime);
	 printf("\tTOTAL CHILD SYS TIME: %ld\n", p_info.cstime);
	 printf("\tUID: %ld\n", p_info.uid);
	 printf("\tPROGRAM NAME: %s\n", p_info.comm);
	 printf("\n-----------END OF CALL---------------\n");
	
}

void create_processes(int num) 
{
	//Store child pids in array.
	pid_t *child_pids = malloc(num * sizeof(pid_t));
	//create n child processes.
	int i;
	for(i = 0; i < num; i++) 
	{
		child_pids[i] = fork();
		//If 0, is child.
		if (child_pids[i] == 0)
		{
			printf("\nCHILD PINFO ID: %d\n", i);
			call_pinfo();
			printf("\nEND CHILD PINFO ID: %d\n", i);
			sleep(2);
			exit(0);
		}
	}
	
	//Wait for all the children created and print out main process's info  before each one.
	pid_t child_pid;
	int child_status;
	while( num > 0)
	{
		printf("\nPARENT SHOULD HAVE %d CHILDREN HERE\n\n", num);
		call_pinfo();
  		child_pid = wait(&child_status);
		--num;
	}
}


void create_threads(int num)
{
	//OpenMP used to create child threads.
	omp_set_num_threads(num);
		
	#pragma omp parallel
	{
		printf("\nTHREAD RUN. THREAD NUM: %d\n", omp_get_thread_num());
		printf("\nEND THREAD RUN\n");

	}

}
