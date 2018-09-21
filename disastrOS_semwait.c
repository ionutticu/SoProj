#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){

	int fd = running->syscall_args[0];
	
	SemDescriptor* descr = SemDescriptorList_byFd(&running->sem_descriptors,fd);
	
	if(!descr){
		running->syscall_retvalue = DSOS_ESEMAPHORENODESC;
		return;
	}

	SemDescriptorPtr* descrptr = descr->ptr;

	if(!descrptr){
		running->syscall_retvalue = DSOS_ESEMAPHORENODESCPTR;
		return;
	}

	Semaphore* sem = descr->semaphore;

	if(!sem){
		running->syscall_retvalue = DSOS_ESEMAPHOREPROBLEM;
		return;
	}
	
	sem->count = (sem->count -1);

	PCB* processo;

	if(sem->count < 0){
		//Boh, funzionerà cosi?
		List_detach(&sem->descriptors,(ListItem*) descrptr);
		List_insert(&sem->waiting_descriptors,sem->waiting_descriptors.last,(ListItem*) descr->ptr);
		List_insert(&waiting_list,waiting_list.last,(ListItem*) running);
		running->status=Waiting;
		//Switch processi
		processo = (PCB*) List_detach(&ready_list,(ListItem*) ready_list.first);
		running = (PCB*) processo;
	
	}
	
	running->syscall_retvalue = 0;

	return;
		
}
