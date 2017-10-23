#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>

int N=10;

producer(){
  while (1) {
	if (N>20) {
		  printf("Overflow!\n");
		  exit(0);
		  };
    	N=N+2;
	printf("Producer: Current N=%d.\n", N);
/*	sleep(1); */
        }
}

consumer() {
  while (1) {
	if (N<=0) {
		  printf("Underflow!\n");
		  exit(0);
		  };
    	N=N-1;
	printf("Consumer: Current N=%d.\n", N);
	sleep(1);
        }
}

main() {
int pid;
	pid = fork();
	if (pid < 0 )
		printf("Fork failure.\n");
	else if (pid > 0)
	{
		printf("Parent PID=%d.\n",pid);
		consumer();
	} else
	{
		printf("Child PID=%d.\n",pid);
		producer();
	}
}
