/*##########################################################
## COP4610 – Principles of Operating Systems – Summer C 2017
## Prof. Jose F. Osorio
## Student Name: Alejandro Palacios – 5156050
##
## Project: Customizing Linux Kernel
## Specs:   Adding Static System Call
## Due Date: 06/12/2017 by 11:55pm
## Module Name: pinfo.c
##
## I Certify that this program code has been written by me
## and no part of it has been taken from any sources.
##########################################################*/

#include<linux/linkage.h>
#include<linux/kernel.h>
#include<linux/pinfo.h>
#include<linux/sched.h>
#include<asm/uaccess.h>
#include<linux/string.h>

struct pinfo *kernel_pinfo;
asmlinkage long sys_pinfo(struct pinfo *info)
{
	//If info struct is null, return -22 error.
	if(!info) 
	{
		return -22;
	}

	kernel_pinfo = kmalloc(sizeof(struct pinfo), GFP_KERNEL);

	//Task structure of current process.
	struct task_struct *task = current;
	
	//Adding all relevant Task Struct properties to the pinfo struct.	
	kernel_pinfo->pid = task->pid;
	kernel_pinfo->state = task->state;
	//Could not find exactly how to get the nice value from prio, but I think this is it.
	kernel_pinfo->nice = task->prio - 120;
	kernel_pinfo->parent_pid = task->parent->pid;
	//Get start time in nanoseconds.
        kernel_pinfo->start_time = task->start_time.tv_nsec;

	//Iterates through every thread and increments thread count.
	int thread_count = 0;
	struct task_struct *temp_thread = task;
	do 
	{

		thread_count++;

	}while_each_thread(task, temp_thread);
	kernel_pinfo->nr_threads = thread_count;
	
	//-1 is default if no children.
	kernel_pinfo->youngest_child_pid = -1;
	kernel_pinfo->cutime = list_empty(&task->children) ? -1 : 0;
	kernel_pinfo->cstime = list_empty(&task->children) ? -1 : 0;
	kernel_pinfo->nr_children = 0;

	long youngest_start_time = 0;
	//Iterates through children. Does a couple of things:
	//1.) calculates cummulative children user and system time.
	//2.) Gets youngest child pid.
	//3.) Gets number of children.
	struct task_struct *temp_children;
        list_for_each_entry(temp_children, &task->children, sibling)
        {
		//Total children user and system time respectively.
		kernel_pinfo->cutime += temp_children->utime;
		kernel_pinfo->cstime += temp_children->stime;

		//If first iteration or current youngest is greater, set the id to the
		//PID being iterated over.
		long child_start_time = temp_children->start_time.tv_nsec;
		
                if(youngest_start_time == 0 || youngest_start_time < child_start_time)
		{
			youngest_start_time = child_start_time;
			kernel_pinfo->youngest_child_pid = temp_children->pid;			

		}

		kernel_pinfo->nr_children++;
        }
	
	kernel_pinfo->younger_sibling_pid = -1;
	kernel_pinfo->older_sibling_pid = -1;
	//Iterates through siblings of the process by getting the parent's children.
	struct task_struct *temp_siblings;
        list_for_each_entry(temp_siblings, &task->parent->children, sibling)
        {	

		//We do not count the current process as a sibling of itself.
		if(kernel_pinfo->pid != temp_siblings->pid) 
		{
			long sibling_start_time = temp_siblings->start_time.tv_nsec;
			//If current process start time is less than sibling, sibling is younger.
			if(kernel_pinfo->start_time < sibling_start_time)
			{
				kernel_pinfo->younger_sibling_pid = temp_siblings->pid;
			}
			//If current process start time is greater than sibling, sibling is older. 
			else if(kernel_pinfo->start_time > sibling_start_time)
			{
				kernel_pinfo->older_sibling_pid = temp_siblings->pid;
			}
		}                
        }	
	
	kernel_pinfo->user_time = task->utime;
	kernel_pinfo->sys_time = task->stime;
	kernel_pinfo->uid = task->uid;
	//String copy current's comm value to the pinfo struct.
	strncpy(kernel_pinfo->comm, task->comm, 16);

	//Test output.
        printk("PINFO START\n");
	printk("\tpid: %d\n", kernel_pinfo->pid);
	printk("\tstate: %ld\n", kernel_pinfo->state);
	printk("\tnice: %ld\n", kernel_pinfo->nice);
	printk("\tparent_pid: %d\n", kernel_pinfo->parent_pid);
	printk("\tnr_children: %d\n", kernel_pinfo->nr_children);
	printk("\tnr_threads: %d\n", kernel_pinfo->nr_threads);
	printk("\tyoungest_child_pid: %d\n", kernel_pinfo->youngest_child_pid);
        printk("\tyounger_sibling_pid: %d\n", kernel_pinfo->younger_sibling_pid);
        printk("\tolder_sibling_pid: %d\n", kernel_pinfo->older_sibling_pid);
        printk("\tstart_time: %ld\n", kernel_pinfo->start_time);
        printk("\tuser_time: %ld\n", kernel_pinfo->user_time);
        printk("\tsys_time: %ld\n", kernel_pinfo->sys_time);
        printk("\tcutime: %ld\n", kernel_pinfo->cutime);
        printk("\tcstime: %ld\n", kernel_pinfo->cstime);
        printk("\tuid: %ld\n", kernel_pinfo->uid);
        printk("\tcomm: %s\n", kernel_pinfo->comm);
        printk("PINFO END\n");


	//Attempt to copy pinfo object back to user space.
	int copy_code = copy_to_user(info, kernel_pinfo, sizeof(struct pinfo));
	
	//Copy to user space pinfo fails, then return EFAULT error code.
	if(copy_code) 
	{
		return -EFAULT;
	}

        return 0;
}
