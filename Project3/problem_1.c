#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
int dog_count;
int cat_count;
int bird_count;
int* pids;
int thread_count;

volatile int running=1;

volatile int curr_cat = 0 ;
volatile int curr_dog = 0 ;
volatile int curr_bird = 0 ;
volatile int play_cat = 0 ;
volatile int play_dog = 0 ;
volatile int play_bird = 0 ;

pthread_t thread_d[100];
pthread_t thread_b[100];
pthread_t thread_c[100];
pthread_mutex_t count_mutex =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t n_cats = PTHREAD_COND_INITIALIZER;
pthread_cond_t n_dogs = PTHREAD_COND_INITIALIZER;
pthread_cond_t n_birds = PTHREAD_COND_INITIALIZER;

static inline int atomic_xadd(volatile int *ptr) {
  register int val __asm__("eax") = 1;
  asm volatile("lock xaddl %0,%1"
  : "+r" (val)
  : "m" (*ptr)
  : "memory"
  );  
  return val;
}

int cond_cats(void)
{
	if((curr_dog == 0) && (curr_bird == 0))
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

int cond_dogs(void)
{
	if(curr_cat == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}
int cond_birds(void)
{
	if(curr_cat == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

void play(void) 
{
  for (int i=0; i<10; i++) 
  {
    assert(curr_cat >= 0 && curr_cat <= cat_count);
    assert(curr_dog >= 0 && curr_dog <= dog_count);
    assert(curr_bird >= 0 && curr_bird <= bird_count);
    assert(curr_cat == 0 || curr_dog == 0);
    assert(curr_cat == 0 || curr_bird == 0);
   }
}

void dog_exit(void)
{
	assert(pthread_mutex_lock(&count_mutex)==0);
	curr_dog--;
	if(curr_dog == 0)
	{
		assert(pthread_cond_broadcast(&n_cats)==0);
	}

	assert(pthread_mutex_unlock(&count_mutex)==0);
}

void cat_exit(void)
{
	assert(pthread_mutex_lock(&count_mutex)==0);
	curr_cat--;
	if(curr_cat == 0)
	{
		assert(pthread_cond_broadcast(&n_dogs)==0);
		assert(pthread_cond_broadcast(&n_birds)==0);
	}

	assert(pthread_mutex_unlock(&count_mutex)==0);

}

void bird_exit(void)
{
	assert(pthread_mutex_lock(&count_mutex)==0);
	curr_bird--;
	if(curr_bird == 0)
	{
		assert(pthread_cond_broadcast(&n_cats)==0);
		
	}

	assert(pthread_mutex_unlock(&count_mutex)==0);

}

void* dog_enter(void* v)
{

	while(running)
	{
		assert(pthread_mutex_lock(&count_mutex)==0);
	while(!cond_dogs())
		{
			
			assert(pthread_cond_wait(&n_dogs,&count_mutex)==0);
			
		}
			
	play_dog++;
	curr_dog++;
	
	assert(pthread_mutex_unlock(&count_mutex)==0);
        
    play();
	
	dog_exit();
	}
	return 0;
}

void* cat_enter(void* v)
{
	while(running)
	{
	assert(pthread_mutex_lock(&count_mutex)==0);
	while(!cond_cats())
		{
		
			assert(pthread_cond_wait(&n_cats,&count_mutex)==0);
			
		}	
	play_cat++;
	curr_cat++;			
	pthread_mutex_unlock(&count_mutex);
        
    play();
	//atomic_xadd(curr_cat);	
	cat_exit();
	}
	return 0;
}

void* bird_enter(void* v)
{
	while(running)
	{
	pthread_mutex_lock(&count_mutex);

	//printf("Hiiiiiiii\n");

	while(!cond_birds())
		{
			//pthread_mutex_unlock(&count_mutex);
			pthread_cond_wait(&n_birds,&count_mutex);
			//pthread_mutex_lock(&count_mutex);
		}
	play_bird++;
	curr_bird++;
		
	pthread_mutex_unlock(&count_mutex);
        
    play();
	//atomic_xadd(curr_bird);	
	bird_exit();
	}
	return 0;
}

int main(int argc, char* argv[])
{	
	int i;
	argc = argc;
	if (argc <4)
		{
            fprintf(stderr, "Expected 3 arguments.\n");
            exit(-1);
        } else {
            dog_count =atoi(argv[1]);
            
            if (dog_count < 0 || dog_count > 99)
            {
                fprintf(stderr, "Invalid dog count.\n");
                exit(-1);
            }
            
            cat_count =atoi(argv[2]);
            
            if (cat_count < 0 || cat_count > 99)
            {
                fprintf(stderr, "Invalid cat count.\n");
                exit(-1);
            }
            
            bird_count =atoi(argv[3]);
            
            if (bird_count < 0 || bird_count > 99)
            {
                fprintf(stderr, "Invalid bird count.\n");
                exit(-1);
            }
        }


	thread_count = dog_count + cat_count + bird_count;	
	
	for(i =0; i<dog_count;i++)
	{
    
		assert(pthread_create(&thread_d[i],NULL,&dog_enter,NULL)==0);
	}
		
	for(i =0; i<cat_count;i++)
	{
     

		assert(pthread_create(&thread_c[i],NULL,&cat_enter,NULL)==0);
	}
	
	for(i =0; i<bird_count;i++)
	{
        
		assert(pthread_create(&thread_b[i],NULL,&bird_enter,NULL)==0);
	}

	sleep(10);
	running=0;
	for(i =0; i<dog_count;i++)
	{
       		assert(pthread_join(thread_d[i],NULL)==0);
	}
	for(i =0; i<cat_count;i++)
	{

		assert(pthread_join(thread_c[i],NULL)==0);
	}
	for(i =0; i<bird_count;i++)
	{
      
		assert(pthread_join(thread_b[i],NULL)==0);
	}
	fprintf(stdout,"Dog play %d, Cat play %d, bird play %d ",play_dog,play_cat,play_bird);
	
	return 0;

}
