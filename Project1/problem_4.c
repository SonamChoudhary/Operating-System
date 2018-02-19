#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

volatile int in_cs=0;
int thread_count =2;
int execution_time =2;
volatile int count_cs =0;

struct spin_lock_t
	{
		volatile int num;
	};


static inline int atomic_cmpxchg (volatile int *ptr, int old, int new) {
  int ret;
  asm volatile ("lock cmpxchgl %2,%1"
    : "=a" (ret), "+m" (*ptr)     
    : "r" (new), "0" (old)      
    : "memory");         
  return ret;                            
}


void spin_lock (struct spin_lock_t *s)
{
	while (atomic_cmpxchg(&(s->num),0,1)==1)
	{}
}

void spin_unlock (struct spin_lock_t *s)
{
	s->num=0;
}

void* cs_function(void *arg)
{	

    struct spin_lock_t * spin= (struct spin_lock_t *) arg; 
	
	while(1)
	{	
    	spin_lock(spin);
    
		assert(in_cs==0);
		in_cs++;
		assert(in_cs==1);
		in_cs++;
		assert(in_cs==2);
		in_cs++;
		assert(in_cs==3);
		in_cs=0;
		count_cs++;
		spin_unlock(spin);
	}
}


int main(int argc, char* argv[])
{	
	struct spin_lock_t spin;
	spin.num =0;
	int i;	
	pthread_t thread[thread_count];
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
	//assert((count =calloc(thread_count,sizeof(int)))!=NULL);
	    
	for(i =0; i<thread_count;i++)
	{
        
		assert(pthread_create(&thread[i],NULL,&cs_function,&spin)==0);
	}	

	sleep(execution_time);
		
	fprintf(stdout,"cs entered %d\n",(int)count_cs);

	return 0;
}
