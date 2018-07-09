#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
	/*Mi "ispiro" all'open_resource fornita
		Prendo gli argomenti dal processo che chiamerà sem_open
		Resource_id mi identifica l'id della risorsa su cui voglio operare
		il count il contatore del semaforo.
	*/
	int resource_id = running->syscall_args[0];
	int count = running->syscall_args[1];
	
	//Vediamo se per caso questo semaforo non esiste già...
	Semaphore* sem = SemaphoreList_byId(&semaphores_list,resource_id);
	
	//L'ho trovato?Esiste?Boh
	
	if(!sem) {
		//Alloco
		sem = Semaphore_alloc(resource_id,count);
		//Inserisco nella lista dei sem
		List_insert(&semaphores_list,semaphores_list.last,(ListItem*) sem);
	}
	//Problemi nel frattempo??
	if(!sem) {
		running->syscall_retvalue = DSOS_ESEMAPHOREPROBLEM;
		return;
	}
	//Devo allocare il sem_descriptor,che prende un fd,un semaforo e un pcb
	
	SemDescriptor* descr = SemDescriptor_alloc(running->last_fd,sem,running);
	//Solita verifica se qualcosa è andato storto
	if(!descr){
		running->syscall_retvalue = DSOS_ESEMAPHORENODESC;
		return;
	}
	
	//A questo punto devo inserire il descrittore nella lista dei descrittori del processo corrente
	List_insert(&running->sem_descriptors,running->sem_descriptors.last,(ListItem*) descr);
	running->last_sem_fd++;
	
	SemDescriptorPtr* descrptr = SemDescriptorPtr_alloc(descr);
	
	//Problemini??
	
	if(!descrptr){
		running->syscall_retvalue = DSOS_ESEMAPHORENODESCPTR;
		return;
	}
	
	//Se tutto ok aggiungo il descrptr a descr->ptr
	descr->ptr = descrptr;
	List_insert(&sem->descriptors,sem->descriptors.last,(ListItem*) descrptr);
	
	//Ritorno,dovrei aver fatto tutto
	
	running->syscall_retvalue = descr -> fd;
	return;	
}

