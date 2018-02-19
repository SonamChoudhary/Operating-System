#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

volatile int in_cs=0;
volatile int* number;
volatile int* entering;
volatile int th =0;
volatile int* count; 
int* pids;
int thread_count=2;
int execution_time=2;
volatile int count_cs =0;

void mfence (void) {
  asm volatile ("mfence" : : : "memory");
}



int max(int* number)
{
	int i,max =0;
	
	for (i=0;i<thread_count;i++)
		{ 
            mfence (); // so that reordering of memory operations doesn't affect threads as at here multiple threads are operating on the same memeory 
			if(number[i] > max)
			{
				max=number[i];
			}

		}
	return max;
}

void lock(int thread_index)
{
    int j;
    entering[thread_index] = 1;
    number[thread_index] = 1 + max((int *)number);
    entering[thread_index]= 0;
    
    for (j =0;j< thread_count;j++)
    {
    	mfence();// so that reordering of memory operations doesn't affect threads as at here multiple threads are operating on the same memeory 
        if (j != thread_index) {
		    while (entering[j])
		    {}
		    while(number[j]!=0 && ((number[thread_index] > number[j])
                                 || (number[thread_index] == number[j] && thread_index > j)))
		    {}
        }
    }
}

void unlock(int thread_index)
{

	number[thread_index]=0;

}

void* cs_function(void *arg)
{	//mfence is not needed since a lock is already acquired before working on the same piece of memory.So memory ordering won't affect.
	int thread = *((int*)arg);
	while(1)
	{	
		lock(thread);
		assert(in_cs==0);
		in_cs++;
		assert(in_cs==1);
		in_cs++;
		assert(in_cs==2);
		in_cs++;
		assert(in_cs==3);
		in_cs=0;
		count_cs++;

        count[thread] ++;
		unlock(thread);
	}

}


int main(int argc, char* argv[])
{	
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
	assert((entering = calloc(thread_count,sizeof(int)))!=NULL);
	assert((count =calloc(thread_count,sizeof(int)))!=NULL);
	assert((number = calloc(thread_count,sizeof(int)))!=NULL);

    assert((pids= malloc(sizeof(int) * thread_count))!=NULL);
	
	
	for(i =0; i<thread_count;i++)
	{
        pids[i] = i;
        assert(pthread_create(&thread[i],NULL,&cs_function,pids+i)==0);
		//pthread_create(&thread[i],NULL,&cs_function,pids+i);
	}	

	sleep(execution_time);
	//th_var = 0;
	for (i=0;i<thread_count;i++)
	{
		fprintf(stdout,"Thread %d enters %d\n",i,count[i]);
	}
	fprintf(stdout,"cs entered %d\n",(int)count_cs);
	return 0;
}
