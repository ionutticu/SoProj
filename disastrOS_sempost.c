#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){

	int fd = running->syscall_args[0];

	SemDescriptor* descr = SemDescriptorList_byFd(&running->sem_descriptors,fd);
	
	//Soliti tre controlli a cascata
	if(!descr){
		running->syscall_retvalue = DSOS_ESEMAPHORENODESC;
		return;
	}
	
	Semaphore* sem = descr->semaphore;
	
	if(!sem){
		running->syscall_retvalue = DSOS_ESEMAPHOREPROBLEM;
		return;
	}


	//Incremento semaforo
	sem->count = (sem->count+1);

	SemDescriptorPtr* proc_desptr;

	if(sem->count == 0){
		List_insert(&ready_list,ready_list.last,(ListItem*) running);
    proc_desptr = (SemDescriptorPtr*) List_detach(&sem->waiting_descriptors, (ListItem*) sem->waiting_descriptors.first);
    List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) proc_desptr);
    List_detach(&waiting_list, (ListItem*) proc_desptr->descriptor->pcb);
    running->status = Ready;
    running = proc_desptr->descriptor->pcb;
	}
	
	running->syscall_retvalue = sem->count;
	return;
}













