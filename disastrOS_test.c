#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "disastrOS.h"

#define NUM_CHILDREN 300
#define BUFFERSIZE 300

void produce(int empty_count,int filled_count,int mutex){
	disastrOS_semwait(empty_count);
	disastrOS_semwait(mutex);

  printf("PRODUCE \n");
  disastrOS_sempost(mutex);
	disastrOS_sempost(filled_count);
}

void consume(int empty_count,int filled_count,int mutex){
	disastrOS_semwait(filled_count);
	disastrOS_semwait(mutex);
 	printf("CONSUME \n");
  disastrOS_sempost(mutex);
	disastrOS_sempost(empty_count);
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
	
  int empty_count = disastrOS_semopen(1,BUFFERSIZE);
	int filled_count = disastrOS_semopen(2,0);
	int mutex = disastrOS_semopen(3,0);
	if(disastrOS_getpid()%2==0){
		printf("I am the children %d and i will produce\n",disastrOS_getpid());
		produce(empty_count,filled_count,mutex);
	}
	if(disastrOS_getpid()%2==1){
		printf("I am the children %d and i will consume\n",disastrOS_getpid());
		consume(empty_count,filled_count,mutex);
	}
	disastrOS_semclose(empty_count);
	disastrOS_semclose(mutex);
	disastrOS_semclose(filled_count);
	printf("EXITING PID:%d\n",disastrOS_getpid()+1);
  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  int alive_children=0;
  for (int i=0; i<NUM_CHILDREN; ++i) {
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){
	alive_children--;
  }
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
