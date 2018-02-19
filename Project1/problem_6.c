#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#define p_count 100
#define MAX 1
#define MIN -1
volatile int in_cir=0;
int thread_count =2;
int execution_time =2;

static inline int atomic_xadd(volatile int *ptr) {
  register int val __asm__("eax") = 1;
  asm volatile("lock xaddl %0,%1"
  : "+r" (val)
  : "m" (*ptr)
  : "memory"
  );  
  return val;
}


inline float Rand_num(float min, float max)
{
	return (max-min) * ((((float) rand())/(float)RAND_MAX))+min;
}

void* generate_rand()
{	int i;
	float x,y;
	for ( i =0;i<p_count;i++)
	{
		x = Rand_num(MIN,MAX);
		y = Rand_num(MIN,MAX);
		if((x*x+y*y)<=1)
		{
			atomic_xadd(&in_cir);
		}
	}
	return 0;
}

int main(int argc, char* argv[])
{	
	float pi_val;
	//spin.num =0;
	int i;
	argc = argc;
	if ((argc>1) && (argc ==2))
		{
			if(atoi(argv[1])<100)
				thread_count =atoi(argv[1]);
			else if(atoi(argv[1])>100)
				thread_count = 100;
		}
	else if ((argc>1) && (argc ==3))
		{
			if(atoi(argv[1])<100)
				thread_count =atoi(argv[1]);
			else if(atoi(argv[1])>100)
				thread_count = 100;
			execution_time = atoi(argv[2]);
		}
	pthread_t thread[thread_count];
	for(i =0; i<thread_count;i++)
	{
      
		assert(pthread_create(&thread[i],NULL,&generate_rand,NULL)==0);
	}	

	sleep(execution_time);
	
	pi_val = 4*((in_cir)/(float)(p_count*thread_count));

	fprintf(stdout,"Value of pi is %f\n",pi_val);
	
	return 0;
}
