#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

int **allocate_two_dementional_array(int row, int column)
{
	int **matrix,i=0;
	matrix=calloc(row,sizeof(int*));
	if (matrix==NULL)
	{
		printf("There is no memory!!!");
		return NULL;
	}
	else
	{
		while (i<row)
		{
			matrix[i]=calloc(column,sizeof(int));
			if (matrix[i]==NULL)
				{
				printf("There is no memory!!!");
				return NULL;
				}
			i++;
		}
	}
	return matrix;
}

void free_two_dementional_array(int **array,int row)
{
	int i;
	for(i = 0; i < row; i++)
		{
		free(array[i]);
		i++;
		}
	free(array);
}

int * allocate_shared_array(int * shared, int *ShmID, int size)
{
	// Create the shared memory segment
	if ((*ShmID = shmget(IPC_PRIVATE,size*sizeof(int),IPC_CREAT|0666)) == -1) 
	{
		perror("shmget: shmget failed");
		exit(1);
	}
	// Attach the shared memory to our matrix
	shared = (int *) shmat(*ShmID, NULL, 0);
	return shared;
}

void free_shared_variables(int *array,int ShmID)
{
	shmdt((void *) array); // Detached
	shmctl(ShmID, IPC_RMID, NULL); // Removed
}

int * allocate_two_dementional_shared_array(int * shared, int *ShmID, int rows, int columns)
{
	int row, column;
	int *matrix;
	 
	// Create the shared memory segment
	*ShmID = shmget(IPC_PRIVATE, sizeof(int)*rows*columns, IPC_CREAT|0666);
	 
	// Attach the shared memory to our matrix
	matrix = (int *)shmat(*ShmID, 0, 0);
	
	// Loop through all elements in the array
	for (row = 0; row < rows; row++)
	{
	    for (column = 0; column < columns; column++)
	    {
	        matrix[row*columns + column] = 0; // Equivalent to matrix[column][row]
	    }
	}
	shared = matrix;
}