#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
	//Prendo parametro dalla syscall,il fd in questo caso
	int fd = running->syscall_args[0];

	SemDescriptor* descr = SemDescriptorList_byFd(&running->sem_descriptors,fd);

	//Qualcosa storto?
	
	if(!descr){
		running->syscall_retvalue = DSOS_ESEMAPHOREPROBLEM;
		return;
	}

	descr = (SemDescriptor*) List_detach(&running->descriptors,(ListItem*) descr);
	
	assert(descr);

	//Bene qui viene il bello

	Semaphore* sem = descr->semaphore;

	SemDescriptorPtr* descrptr = (SemDescriptorPtr*) List_detach(& sem->descriptors,(ListItem*)descr->ptr);

	assert(descrptr);

	SemDescriptor_free(descr);
	SemDescriptorPtr_free(descrptr);
	
	//Tutto ok,posso tornare

	running->syscall_retvalue = 0;
  
}
