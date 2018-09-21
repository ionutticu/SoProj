#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "disastrOS.h"

#define NUM_THREADS 3
#define CICLES 5
#define PRODUCERS 1
#define CONSUMERS 1

void produce(int prod_id,int cons_id,int iterations){
    for (int i = 0; i < iterations; i++){
        disastrOS_semwait(prod_id);
        printf("PRODUCE\n");
        disastrOS_sempost(cons_id);
    }
}

void consume(int prod_id,int cons_id,int iterations){
    for (int i = 0; i < iterations; i++){
        disastrOS_semwait(cons_id);
        printf("CONSUME\n");
        disastrOS_sempost(prod_id);
    }
}
// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("\nHello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Thread %d\n",disastrOS_getpid());
 /*-------------------------------------------------------*/
	//printf("1-I am opening the semaphores...!!!\n");
	int prod_fd = disastrOS_semopen(2,PRODUCERS);
	int cons_fd = disastrOS_semopen(1,0);
	

	printf("PROD_FD:%d CONS_FD:%d\n",prod_fd,cons_fd);
	if(disastrOS_getpid() == 3){
		produce(prod_fd,cons_fd,CICLES);
	}
	if(disastrOS_getpid() == 4){
		consume(prod_fd,cons_fd,CICLES);
	}
	disastrOS_sleep(40);

	disastrOS_semclose(prod_fd);
	disastrOS_semclose(cons_fd);

	//disastrOS_printStatus();

  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  printf("  [2].Init function\n");
  disastrOS_spawn(sleeperFunction, 0);
  

  printf("   [3].Spawning some threads.Exactly %d\n",NUM_THREADS);
  int alive_children=0;
  for (int i=0; i<NUM_THREADS; ++i) {
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    printf("Child %d terminated, retval:%d, alive: %d \n",
	  pid, retval, --alive_children);
  }
	disastrOS_sleep(50);
  printf("   [200].Arresto\n");
	disastrOS_printStatus();
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  printf("[1].Start DisastrOS\n"); //[1]
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
