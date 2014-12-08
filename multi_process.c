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
#include "time_Calculation.h"
#include "allocation.h"

/*
// Programmer Note:
// Shared Memory Is Used So (shmget)
// You May need to change SHMMAX
// Because In Ubuntu MAX is 33.55 Mb (Depends On System)
// Location:/proc/sys/kernel/
// FileName:shmmax
*/

//	DEFINE YOUR OPTIONS
#define GET_DATA_FROM_CONSOLE false
#define ARRAY_SIZE 5000000
#define RANGE_SIZE 200
#define TEST_ALL
//#define PRINT_ARRAY

pid_t *pid_Array;

// Semaphores
sem_t *wait_for_all; // First Phrase 
sem_t *mutex;
int ShmID_wait_for_all, ShmID_mutex;

// General Variables
int * array;
int array_size;
int range;
int ShmID_array,ShmID_array_size,ShmID_range;

int * integer_Values; //Hold # of deferent integer
int * temp_Values; //Hold every threads value
int ShmID_integer_Values, ShmID_temp_Values;

int *waiting_threads;
int ShmID_waiting_threads;

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

void thread_function(void * who_i_am)
{
	int i,j; // For Loops
	int number=*(int*)who_i_am; // For Specific Search
	int start = (number*(array_size/range)); 
	int end = (number+1)*(array_size/range);
	if (number==range-1)
		end=array_size;
	
	//First Phrase (Find Integer Values)
	for (i=start; i < end; ++i)
			++(temp_Values[number*range + array[i]]);

	//Wait Others Or Wake All
	sem_wait(mutex);
	if (*waiting_threads == (range-1) )
	{
		printf("First Phrase is Complete.\n");

		//Get Info From All Threads
		for (i = 0; i < range; ++i)
		{
			for (j = 0; j < range; ++j)
			{
				integer_Values[j]+=temp_Values[i*range+j];
			}
		}

		//Wake Up Other Threads 
		for (i = 0; i < range-1; ++i)
			{
				sem_post(wait_for_all);
			}
		sem_post(mutex);
	}
	else
	{
		(*waiting_threads)++;
		sem_post(mutex);
		sem_wait(wait_for_all);
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

void call_process()
{
	int i;
	int * who_i_am=(int *) malloc (range*sizeof(int));

	//Call Threads
	for (i = 0; i < range; i++)
	{
		*(who_i_am+i)=i;
		if ((pid_Array[i] = fork()) == 0)
		{
			thread_function(who_i_am+i);
			exit(0);
		}
	}

	///Wait Others
	int status;
	pid_t pid;
	int n =range;
	while (n > 0) 
	{
	  	pid = wait(&status);
	  	--n;
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
	printf("Creating Shared Array(Size=%d)...\n",array_size);
	array = allocate_shared_array( array,&ShmID_array,array_size);

	printf("Creating PID Array(Size=%d)...\n",range);
	pid_Array = (pid_t *)calloc(range,sizeof(pid_t));

	printf("Creating Values...\n"); // For existing exist
	integer_Values = allocate_shared_array( integer_Values,&ShmID_integer_Values,range);
	temp_Values = allocate_two_dementional_shared_array( temp_Values, &ShmID_temp_Values ,range,range);


	ShmID_waiting_threads = shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT|0666);
	waiting_threads = (int *) shmat(ShmID_waiting_threads, NULL, 0);

	ShmID_mutex = shmget(IPC_PRIVATE,sizeof(sem_t),IPC_CREAT|0666);
	mutex = (sem_t *) shmat(ShmID_mutex, NULL, 0);
	ShmID_wait_for_all = shmget(IPC_PRIVATE,sizeof(sem_t),IPC_CREAT|0666);
	wait_for_all = (sem_t *) shmat(ShmID_wait_for_all, NULL, 0);

	//Randomize
	srand(time(NULL));
	printf("Making It Random(Size=%d)...\n",array_size);
	for (i = 0; i < array_size; i++)
	{
		array[i] =rand() % range;
	}



	//Initialize Semaphores (Process -> 2nd input is 1)
	sem_init(wait_for_all,1,0);
	sem_init(mutex,1,1);

	//CALL THE FONCTION
	timeCalculation(&call_process);

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
	free(pid_Array);

	free_shared_variables(array, ShmID_array);
	free_shared_variables(integer_Values, ShmID_integer_Values);
	free_shared_variables(temp_Values, ShmID_temp_Values);
	free_shared_variables(waiting_threads, ShmID_waiting_threads);

	shmdt((void *) mutex); // Detached
	shmctl(ShmID_mutex, IPC_RMID, NULL); // Removed
	shmdt((void *) wait_for_all); // Detached
	shmctl(ShmID_wait_for_all, IPC_RMID, NULL); // Removed
	return 0;
}