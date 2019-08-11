#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


struct read_write_lock
{
	// If required, use this strucure to create
	// reader-writer lock related variables
	pthread_mutex_t mutex;	
	pthread_cond_t  reader_cond;
	pthread_cond_t  writer_cond;
	int reader_count;
	int writer_count;
	int waiting_writer_count;

}rwlock;

long int data = 0;			//	Shared data variable

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
	rw->reader_count = 0;
	rw->writer_count = 0;
	rw->waiting_writer_count = 0;
	pthread_mutex_init(&(rw->mutex),NULL);
	pthread_cond_init(&(rw->reader_cond),NULL);
	pthread_cond_init(&(rw->writer_cond),NULL);
}


// The pthread based reader lock
void ReaderLock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&(rw->mutex));
	while(rw->waiting_writer_count > 0 || rw->writer_count > 0)
		pthread_cond_wait(&(rw->reader_cond),&(rw->mutex));
	rw->reader_count++;
	pthread_mutex_unlock(&(rw->mutex));
}	

// The pthread based reader unlock
void ReaderUnlock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&(rw->mutex));
	rw->reader_count--;
	if(rw->reader_count == 0)
		pthread_cond_broadcast(&(rw->writer_cond));
	pthread_mutex_unlock(&(rw->mutex));
}

// The pthread based writer lock
void WriterLock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&(rw->mutex));
			rw->waiting_writer_count ++;
	while(rw->reader_count > 0 || rw->writer_count > 0)
	{

		pthread_cond_wait(&(rw->writer_cond),&(rw->mutex));

	}
			rw->waiting_writer_count--;
	rw->writer_count ++;
	pthread_mutex_unlock(&(rw->mutex));
}

// The pthread based writer unlock
void WriterUnlock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&(rw->mutex));
	rw->writer_count --;
	pthread_cond_broadcast(&(rw->writer_cond));
	pthread_cond_broadcast(&(rw->reader_cond));
	pthread_mutex_unlock(&(rw->mutex));
}

//	Call this function to delay the execution by 'delay' ms
void delay(int delay)
{
	struct timespec wait;

	if(delay <= 0)
		return;

	wait.tv_sec = delay / 1000;
	wait.tv_nsec = (delay % 1000) * 1000 * 1000;
	nanosleep(&wait, NULL);
}

struct argument_t
{
	int id,delay_time;
};

// The pthread reader start function
void *ReaderFunction(void *args)
{
	//	Delay the execution by arrival time specified in the input
	struct argument_t *arg = (struct argument_t *)args;
	//	....
	delay(arg->delay_time);
	
	//  Get appropriate lock
	ReaderLock(&rwlock);
	//	Display  thread ID and value of the shared data variable
	//
	printf("\tReader\t%d, data: %ld\n",arg->id,data);
    //  Add a dummy delay of 1 ms before lock release  
	//	....
	delay(1);
	ReaderUnlock(&rwlock);
}

// The pthread writer start function
void *WriterFunction(void *args)
{
	//	Delay the execution by arrival time specified in the input
	struct argument_t *arg = (struct argument_t *)args;
	//	....
	delay(arg->delay_time);
	//
	//  Get appropriate lock
	WriterLock(&rwlock);
	//	Increment the shared data variable
	data++;
	//	Display  thread ID and value of the shared data variable
	//
	printf("Writer\t%d, data: %ld\n",arg->id,data);
    //  Add a dummy delay of 1 ms before lock release  
	//	....
	delay(1);
	WriterUnlock(&rwlock);
	
}

int main(int argc, char *argv[])
{
	pthread_t *threads;
	struct argument_t *arg;
	
	long int reader_number = 1;
	long int writer_number = 1;
	long int thread_number = 0;
	long int total_threads = 0;	
	
	int count = 0;			// Number of 3 tuples in the inputs.	Assume maximum 10 tuples 
	int rws[10];			
	int nthread[10];
	int delay[10];

	//	Verifying number of arguments
	if(argc<4 || (argc-1)%3!=0)
	{
		printf("reader-writer <r/w> <no-of-threads> <thread-arrival-delay in ms> ...\n");		
		printf("Any number of readers/writers can be added with repetitions of above mentioned 3 tuple \n");
		exit(1);
	}

	//	Reading inputs
	for(int i=0; i<(argc-1)/3; i++)
	{
		char rw[2];
		strcpy(rw, argv[(i*3)+1]);

		if(strcmp(rw, "r") == 0 || strcmp(rw, "w") == 0)
		{
			if(strcmp(rw, "r") == 0)
				rws[i] = 1;					// rws[i] = 1, for reader
			else
				rws[i] = 2;					// rws[i] = 2, for writer
			
			nthread[i] = atol(argv[(i*3)+2]);		
			delay[i] = atol(argv[(i*3)+3]);

			count ++;						//	Number of tuples
			total_threads += nthread[i];	//	Total number of threads
		}
		else
		{
			printf("reader-writer <r/w> <no-of-threads> <thread-arrival-delay in ms> ...\n");
			printf("Any number of readers/writers can be added with repetitions of above mentioned 3 tuple \n");
			exit(1);
		}
	}

	//	Create reader/writer threads based on the input and read and write.
	threads = malloc(total_threads*sizeof(pthread_t));
	arg = malloc(total_threads*sizeof(struct argument_t));

	for(int i=0;i<count;i++)
	{
		if(rws[i] == 1) // Reader
		{
			for(int j=0;j<nthread[i];j++)
			{
				arg[thread_number].delay_time = delay[i];
				arg[thread_number].id = reader_number;
				pthread_create(&threads[thread_number], NULL, ReaderFunction, (void*) &arg[thread_number]);
				reader_number++;
				thread_number++;
			}
		}
		else // Writer
		{
			for(int j=0;j<nthread[i];j++)
			{
				arg[thread_number].delay_time = delay[i];
				arg[thread_number].id = writer_number;
				pthread_create(&threads[thread_number], NULL, WriterFunction, (void*) &arg[thread_number]);
				writer_number++;
				thread_number++;
			}
		}
	}

	//  Clean up threads on exit
	for(int i=0; i<total_threads; i++)
	{
		//	Wait for all threads to finish
		pthread_join(threads[i], NULL);
	}

	exit(0);
}
