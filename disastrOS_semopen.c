#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
	//Mi "ispiro" all'open_resource fornita
	//Prendo gli argomenti dal processo che chiamerà sem_open
	int resource_id = running->syscall_args[0];
	int count = running->syscall_args[1];
}
