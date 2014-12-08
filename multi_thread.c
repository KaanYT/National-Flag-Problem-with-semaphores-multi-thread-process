/*-----------------------*/ 
/* Created by Kaan Eksen */ 
/*-----------------------*/ 
/*-----------------------*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h> 
#include <stdbool.h>
#include <pthread.h> 
#include <stdbool.h>
#include "allocation.h"
#include "time_Calculation.h"

//	DEFINE YOUR OPTIONS
#define GET_DATA_FROM_CONSOLE true
#define ARRAY_SIZE 50000000
#define RANGE_SIZE 200
#define TEST_ALL
//#define PRINT_ARRAY

pthread_mutex_t mymutex=PTHREAD_MUTEX_INITIALIZER;
pthread_t *all_threads;

// Semaphores
sem_t wait_for_all; // First Phrase 
sem_t mutex; // Mutex

// General Variables
int * array;
int array_size;
int range;

int * integer_Values; //Hold # of deferent integer
int ** temp_Values; //Hold every threads value

int waiting_threads=0;

void print_array(int *array,int size)
{
	int i;
	printf("Array: ");
	for (i = 0; i < size; ++i)
	{
		printf("%d ", array[i]);
	}
	printf("\n");
}

void *thread_function(void * who_i_am)
{
	int i,j; // For Loops
	int number=*(int*)who_i_am; // For Specific Search

	//Calculate Search Space
	int start = (number*(array_size/range)); 
	int end = (number+1)*(array_size/range);
	if (number==range-1)
		end=array_size;

	//First Phrase (Find Integer Values)
	for (i=start; i < end; ++i)
		++(temp_Values[number][array[i]]);

	//Wait Others Or Wake All
	sem_wait(&mutex);
	if (waiting_threads == (range-1) )
	{
		printf("First Phrase is Complete.\n");
		//Get Info From All Threads

		for (i = 0; i < range; ++i)
		{
			for (j = 0; j < range; ++j)
			{
				integer_Values[j]+=temp_Values[i][j];
			}
		}

		//Wake Up Other Threads 
		for (i = 0; i < range-1; ++i)
			{
				sem_post(&wait_for_all);
			}
		sem_post(&mutex);
	}
	else
	{
		waiting_threads++;
		sem_post(&mutex);
		sem_wait(&wait_for_all);
	}

	//Second Phrase (Rearrange Array)
	start=0;
	for (i = 0; i < number; ++i)
	{
		start = start+integer_Values[i];
	}
	end=start+integer_Values[i];

	for (i=start; i < end; ++i)
	{
		array[i]=number;
	}
}

void call_threads()
{
	int i;
	int * who_i_am=(int *) malloc (range*sizeof(int));

	//Call Threads
	for (i = 0; i < range; i++)
	{
		*(who_i_am+i)=i;
		if (pthread_create( &(all_threads[i]), NULL, thread_function, (void*)(who_i_am+i)) )
		{
	    	printf("error creating thread.(%d)",i);
			abort();
		}
	}

	//Wait Threads
	for (i = 0; i < range; i++)
	{
		pthread_join(all_threads[i], NULL);
	}

	//Clean Up
	free(who_i_am);
}

int main(int argc, char const *argv[])
{
	int i;
	array_size = ARRAY_SIZE;
	range = RANGE_SIZE;

	if (GET_DATA_FROM_CONSOLE)
	{
		if (argc <= 2)
		{
			printf("Invalid input\n");
			printf("You Have to Enter 2 Input\n");
			printf("1-)Size Of Array\n");
			printf("2-)Range Of Numbers\n");
			printf("Exiting...\n");
			exit(0);
		}
		//Get Info
		array_size = atoi(argv[1]);
		range = atoi(argv[2]);
	}

	//Create
	printf("Creating threads(Size=%d)...\n",range);
	all_threads = (pthread_t *)calloc(range,sizeof(pthread_t));

	printf("Creating Array(Size=%d)...\n",array_size);
	array = (int *)malloc(sizeof(int)*array_size);

	printf("Creating Values...\n"); // For existing exist
	integer_Values = (int *)calloc(range,sizeof(int));
	temp_Values = allocate_two_dementional_array(range,range);
	print_array(temp_Values[0],range);

	//Randomize
	srand(time(NULL));
	printf("Making It Random(Size=%d)...\n",ARRAY_SIZE);
	for (i = 0; i < array_size; i++)
	{
		array[i] =rand() % range;
	}

	//Initialize Semaphores (Thread -> 2nd input is 0)
	sem_init(&wait_for_all,0,0); 
	sem_init(&mutex,0,1);

	//CALL THE FONCTION
	timeCalculation(&call_threads);

	#ifdef TEST_ALL
	printf("Testing Array...\n");
	for (i = 0; i < array_size-1; ++i)
	{
		if (array[i]>(array[i+1]) )
		{
			printf("Problem Found In shared_Array[%d]\n",i);
			break;
		}
	}
	if (i==array_size-1)
		printf("No Problem Found!!!\n");
	#endif

	#ifdef PRINT_ARRAY
	print_array(array,array_size);
	#endif

	//Clean Up Your Mess
	free_two_dementional_array(temp_Values,range);
	free(array);
	free(integer_Values);
	free(all_threads);

	return 0;
}
