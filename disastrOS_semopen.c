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
	Semaphore* sem = SemaphoreList_byId(&semaphores_list,id);
	
	//L'ho trovato?Esiste?Boh
	
	if(!sem) {
		//Alloco
		sem = Semaphore_alloc(id,count);
		//Inserisco nella lista dei sem
		List_insert(&semaphores_list,semaphores_list.last,(ListItem*) sem);
	}
	//Problemi nel frattempo??
	if(!sem) {
		running->syscall_retvalue = DSOS_ESEMAPHOREPROBLEM;
		return;
	}
}
