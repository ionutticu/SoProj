#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "disastrOS.h"

#define QUANTI 20
#define PRODUTTORI 2
// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int type=0;
  int mode=0;
  int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  printf("fd=%d\n", fd);

	int i;
	


 /*-------------------------------------------------------*/
	printf("1-I am opening the semaphores...!!!\n");
	int prod_fd = disastrOS_semopen(1,PRODUTTORI);
	int cons_fd = disastrOS_semopen(2,0);
	
	printf("2-Semahores opened\n");

	//Produco
	if(disastrOS_getpid() == 4){
		for(i=0;i<QUANTI;i++){
			disastrOS_semwait(prod_fd);
			printf("Produco\n");
			disastrOS_sempost(cons_fd);
		}
	}
	if(disastrOS_getpid() == 3){
		for(i=0;i<QUANTI;i++){
			disastrOS_semwait(cons_fd);
			printf("Consumo\n");
			disastrOS_sempost(prod_fd);
		}
	}
	disastrOS_sleep(20);
  printf("PID: %d, terminating\n", disastrOS_getpid());
	
	
	disastrOS_printStatus();

  /*for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
  }*/
	disastrOS_semclose(prod_fd);
	printf("CiaoCiao");
	disastrOS_semclose(cons_fd);

	disastrOS_printStatus();

  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  

  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;
  for (int i=0; i<10; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }
  printf("shutdown!");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
