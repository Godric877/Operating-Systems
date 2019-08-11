#include <pthread.h>
#include <stdio.h>

int counter_array[10];
pthread_mutex_t mutex_array[10];

#define NTHREADS 10

void init_counters()
{
	for(int i=0;i<10;i++)
		counter_array[i] = 0;
}

void init_mutex()
{
	for(int i=0;i<10;i++)
		pthread_mutex_init(&mutex_array[i],NULL);
}

void *increment_counter(void* index)
{
	int* ptr = (int*) index;
	int i = *ptr;
	for(int j=0;j<1000;j++)
	{
		pthread_mutex_lock(&mutex_array[i]);
		counter_array[i]++;	
		pthread_mutex_unlock(&mutex_array[i]);
	}
}

int main()
{

	init_counters();
	init_mutex();

	pthread_t tid[NTHREADS];

	int index_array[10];

	for(int i=0;i<10;i++)
	{
		index_array[i] = i;
	}

	for(int i=0; i<NTHREADS; i++)
	{
		//	Create a thread with default attributes and no arguments
		pthread_create(&tid[i], NULL, increment_counter, (void*) &index_array[i]);
	}

	for(int i=0;i<1000;i++)
	{
		for(int j=0;j<10;j++)
		{
		pthread_mutex_lock(&mutex_array[j]);
		counter_array[j]++;
		pthread_mutex_unlock(&mutex_array[j]);
		}
	}

	for(int i=0; i<NTHREADS; i++)
	{
		//	Wait for all threads to finish
		pthread_join(tid[i], NULL);
	}

	for(int i=0;i<10;i++)
	{
		printf("Counter[%d] = %d\n",i,counter_array[i]);
	}
}