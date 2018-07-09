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


	//Incremento semafor
	sem->count = (sem->count+1);

	if(sem->count >= 0){

		List_detach(&sem->waiting_descriptors,(ListItem*) descr);
		List_insert(&sem->descriptors,sem->descriptors.last,(ListItem*)descr);	

		//Tolgo dalla waiting list il processo bloccato dal sem,descrptr->pcb
		//Lo inserisco in ready list
		

		List_detach(&waiting_list,(ListItem*) descr->pcb);
		List_insert(&ready_list,ready_list.last,(ListItem*) descr->pcb);
	}
	
	running->syscall_retvalue = 0;
	return;
}













