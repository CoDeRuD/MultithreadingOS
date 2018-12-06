/***************************************************************************************************************
     PART 1: assign4-part1.c written by Lavell Townsel

Purpose:
     Practice threads with synchronization by implementing a threading problem referred to as, "The Dining 
     Philosophers Problem". Use multithreading techniques for the creation of simple threads

Command Parameters:
     gcc -pthread -o assign4p1 assign4-part1.c
     ./assign4p1 <nthreads>
     
     steps to compile & execute this program

Results:
     *** Please refer to the Report.txt file for further information ***

Notes:
     N/A
Returns:
     N/A
***************************************************************************************************************/
#include <stdio.h>//basic functionality of C Program
#include <stdlib.h>//exit()
#include <string.h>//string manipulation
#include <errno.h>//error handling
#include <pthread.h>//usage of thread functions


/******************************************** philisopherThread ********************************************
Purpose:
     prints out a statment for each of the philosopher index threads whenever encountered in the program.

Parameters:
     O     void *pVoid                          pointer to an integer to be recognized as index of thread

Notes:
     - void *pVoid cast as (int *) type and assigned to an integer variable (ndxPhilosopher) in order to 
       indicate maximum thread index value for any particular philosopher

Returns:
     pthread_exit(NULL)                         exits thread function with null return value
************************************************************************************************************/
void *philosopherThread(void *pVoid)
{
    int ndxPhilosopher = *((int *) pVoid);         // pointer to integer representing index of thread
    //printf("Thread value: %d\n", ndxPhilosopher);  // keep track of threads
     
    int ndx;                                       // thread index
    /* FOR LOOP: for each philosopher thread, a statement is printed indicating
                 which philosopher thread has been encountered */
    //for (ndx = 0 ; ndx < ndxPhilosopher; ++ndx) 
    printf("This is philosopher %d\n", ndxPhilosopher);
    pthread_exit(NULL);                            // thread returns NULL
}


/******************************************* creatPhilosopher *****************************************
Purpose:
     creates the threads, joins the threads, and prints a confirmation message once all threads have
     been created and joined successfully

Parameters:
     I    int nthreads                               number of threads to be created

Notes:
     - called by main()

Returns:
     return;                                         function returns as void
*******************************************************************************************************/
void creatPhilosophers(int nthreads)
{
    pthread_t tid[nthreads];   // initialize thread id as array
    int error;                 // error status variable
     
    int ndx;                   // thread index
    /* FOR LOOP: for each philosopher thread, create them based on the thread function (philosopherThread) 
                 defined earlier which will print each philosopher thread; use the ndx variable as the
		 argument to be passed to philosopherThread */
    for (ndx = 0 ; ndx < nthreads ; ++ndx) {
        int *args = (int*)malloc(sizeof(int));
        *args = ndx + 1;
        /* IF-CONDITION: check status of pthread_create and print an associated error message if
	                 creation failed */
        if (error = pthread_create(&tid[ndx], NULL, philosopherThread, (void *) args))
	    fprintf(stderr, "Failed to create thread: %s\n", strerror(error));
    }
    /*FOR LOOP: for each philosopher thread, join the created threads by passing each philosopher thread
                to pthread_join(...) */
    for (ndx = 0 ; ndx < nthreads ; ++ndx) {
        /* IF-CONDITION: check status of pthread_join abd print an associated error message if 
                         join failed */
        if (error = pthread_join(tid[ndx], NULL))
	    fprintf(stderr, "Failed to join thread: %s\n", strerror(error));
    }

    printf("%d threads have been completed/joined successfully!\n", nthreads); // print complete msg
    return;
}


/******************************************* main ****************************************************
Purpose:
     takes a single command line parameter (an integer) to indicate the number of threads to be 
     created. Calls creatPhilosophers(...) function processing threads passed as input parameter
     by the user

Parameters:
     I      int argc                            number of command line args
     O      char **argv                         pointer to value of command line arg

Notes:
     - uses atoi() to retrieve value of nthreads

Returns:
     0                                          function returns successfully
******************************************************************************************************/
int main(int argc, char **argv) 
{ 
    if (argc != 2) {   /* check correct number of args passed to program */
        fprintf(stderr, "Usage: %s <nthreads>\n", argv[0]);
	exit(1);
    }
    int nthreads;
    nthreads = atoi(argv[1]);      // convert the first argument to an integer and assign to nthreads
    printf("Lavell Townsel - ");
    printf("Assignment 4: # of threads = %s\n", argv[1]);
    creatPhilosophers(nthreads);   // call creatPhilosophers
    return 0;
}
