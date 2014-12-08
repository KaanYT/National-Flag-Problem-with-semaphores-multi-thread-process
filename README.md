National-Flag-Problem-with-semaphores-multi-thread-process
==========================================================

National Flag Problem with semaphores multi-thread and multi-process

Implementation Based on Linux semaphores

How to run:
	gcc multi_thread.c -lpthread -o Thread

	./Thread

Options:
	GET_DATA_FROM_CONSOLE
		You can dynamically enter Array size and range of numbers as console argument.
	ARRAY_SIZE/RANGE_SIZE
		If console argument is false defined values are gonna be used.
	TEST_ALL
		Test the result.
	PRINT_ARRAY
		Print array after completing the task.

Program:
	Create random array based on range size.
	Divide array to equal size for each thread.
	In first cycle find how many of each number and record them.
	Last thread combine all results.
	In second cycle put data to proper place.

Results:
	In 50M Array
		Multi Process: 106.9 ms
		Multi Thread: 84.5 ms

	Test System:
		CPU: i7-2670QM @ 2.20
		RAM: 16GB
	
