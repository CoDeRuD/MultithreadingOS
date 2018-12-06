#include <stdio.h>//basic functionality of C Program
#include <stdlib.h>//exit(..),srandom()
#include <string.h>//strerror()
#include <errno.h>//error handling
#include <unistd.h>//usleep()
#include <time.h>//srandom()
#include <pthread.h>//POSIX thread, POSIX Mutex Usage & Operations


// Globals & Constants
int nthreads;                                                           // global num threads passed as first arg
#define MAX_PHILOSOPHERS 50
pthread_mutex_t left_right_chopsticks[MAX_PHILOSOPHERS] = PTHREAD_MUTEX_INITIALIZER;    // mutex locks for multiple chopsticks
                                                                                        // same as number of philosophers sitting at table


// Prototype Declarations
void thinking();
int pickUpChopsticks(int threadIndex);
void eating();
void putDownChopsticks(int threadIndex);
void *philosopherActivity(void *threadIndex);


/******************************************* main ****************************************************
Purpose:
     process activities of the philosopher threads

Parameters:
     I      int argc                            number of command line args
     O      char **argv                         pointer to value of command line arg

Notes:
     - main processing thread

Returns:
     0                                          function returns successfully
     -1                                         function returns with error
******************************************************************************************************/
int main(int argc, char **argv)
{
    if (argc != 2) {   /* check correct number of args passed to program */
        fprintf(stderr, "Usage: %s <nthreads>\n", argv[0]);
        return -1;
    }
    nthreads = atoi(argv[1]);          // convert first argument to an integer and store in var nthreads
    pthread_t tids[nthreads];          // thread array


    int ndxThread, error;              // thread index & return value for mutex operations
    /* FOR LOOP: threads have to initialize their mutex lock before accessing the mutex for
                 accessing chopsticks */
    for (ndxThread = 0; ndxThread < nthreads; ++ndxThread) {

        /* IF-CONDITION: check status of pthread_mutex_init and print an associated error message if
                         mutex init failed */
        if (error = pthread_mutex_init(&left_right_chopsticks[ndxThread], NULL)) {
	          fprintf(stderr, "Failed to initialize mutex - left_right_chopsticks: %s\n", strerror(error));
	          return -1;
	      }
    }


    /* FOR LOOP: for each philosopher thread, create them using the arguments passed in from
                 threading routine */
    for (ndxThread = 0 ; ndxThread < nthreads ; ++ndxThread) {
        int *args = (int *)malloc(sizeof(int));
        *args = ndxThread + 1;
        // Initialize thread attributes for each thread
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        /* IF-CONDITION: check status of pthread_create and print an associated error message if
                         creation failed */
        if (error = pthread_create(&tids[ndxThread], &attr, philosopherActivity, (void *) args)) {
	         fprintf(stderr, "Failed to create thread: %s\n", strerror(error));
	          return -1;
	      }
    }


    /*FOR LOOP: for each philosopher thread, join the created threads by passing each philosopher thread
                to pthread_join(...) */
    for (ndxThread = 0 ; ndxThread < nthreads ; ++ndxThread) {

        /* IF-CONDITION: check status of pthread_join and print an associated error message if
                         join failed */
        if (error = pthread_join(tids[ndxThread], NULL)) {
	         fprintf(stderr, "Failed to join threads: %s\n", strerror(error));
	         return -1;
	      }
        pthread_mutex_destroy(&left_right_chopsticks[ndxThread]);   // destroy mutex resources
    }

    return 0;
}


/***************************************** thinking ************************************************
Purpose
     process the philosophers' action of thinking prior to picking up the chopsticks

Parameters    None
Notes
     - uses srandom() & random() to seed random number generator, and assign a random number
       for overall microseconds that a philosopher is thinking, respectively

Returns       None
****************************************************************************************************/
void thinking()
{
    unsigned int usecs;                  // unsigned integer type denoting microseconds
    int error;                           // error status

    srandom(time(0));                    // seed random number generator
    usecs = random() % 500 + 1;          // retrieve value between 1 & 500
    /* IF CONDITION: check status of usleep sleep function attempting to pass the calculated
                     random usecs value; Print an associated error message if function returns
                     -1 */
    if (error = usleep(usecs))
        fprintf(stderr, "Failed to start thinking: %s\n", strerror(error));
}


/***************************************** pickUpChopsticks  ***************************************
Purpose
     process the philosophers' action of picking up chopsticks right before eating

Parameters
     I     threadIndex                            current philosopher thread

Notes
     - chopsticks considered a "shared resource"
     - lock the left and right chopsticks using a mutex lock
     - philosopher must acquire both locks before commencing eating

Returns
     0                                          function returns successfully
     -1                                         function returns with error
****************************************************************************************************/
int pickUpChopsticks(int threadIndex)
{
    int c1 = threadIndex - 1; int c2 = threadIndex;
    int error;
    /* IF CONDITION: last philosopher picks up the chopstick to their right, which would be the first
                     philosopher's left one */
    if (c2 >= nthreads)
        c2 = 0;
    if (error = pthread_mutex_trylock(&left_right_chopsticks[c1]))
        fprintf(stderr, "P %d fails to pick up chopsticks %d and %d: %s\n", threadIndex, c1, c2, strerror(error));

    if (error = pthread_mutex_trylock(&left_right_chopsticks[c2])) {
        pthread_mutex_unlock(&left_right_chopsticks[c1]);
        return -1;
    }

    printf("P %d now picking up their chopsticks %d and %d\n\n", threadIndex, c1, c2);
    return 0;
}


/***************************************** eating **************************************************
Purpose
     process the philosophers' action of eating directly after picking up chopsticks and before
     putting down the chopsticks

Parameters    None
Notes
     - uses srandom() & random() to seed random number generator, and assign a random number
       for overall microseconds that a philosopher is eating, respectively

Returns       None
****************************************************************************************************/
void eating()
{
    unsigned int usecs;                  // unsigned integer type denoting microseconds
    int error;                           // error status

    srandom(time(0));                    // seed random number generator
    usecs = random() % 500 + 1;          // retrieve random number between 1 & 500
    /* IF CONDITION: check status of usleep sleep function attempting to pass the calculated
                     random usecs value; Print an associated error message if function returns
                     a nonzero value */
    if (error = usleep(usecs))
        fprintf(stderr, "Failed to start eating: %s\n", strerror(error));

    return;
}


/***************************************** putDownChopsticks ***************************************
Purpose
     philosopher releases locks on the chopsticks; exits

Parameters
     I      threadIndex                         current philosopher thread

Notes           None
Returns         None
****************************************************************************************************/
void putDownChopsticks(int threadIndex)
{
    int c1 = threadIndex - 1; int c2 = threadIndex;
    /* last philosopher releases the chopstick to their right, which would be the first
       philosopher's left one */
    if (c2 >= nthreads)
        c2 = 0;
    pthread_mutex_unlock(&left_right_chopsticks[c1]);
    pthread_mutex_unlock(&left_right_chopsticks[c2]);

    printf("P %d now releasing their chopsticks %d and %d\n\n", threadIndex, c1, c2);
    return;
}


/****************************************** philosopherActivity **************************************
Purpose
    thread routine for pthread_create and implements the four different actions taken by the
    philosophers:  thinking, picking up chopsticks, eating, & putting down chopsticks

Parameters
    I      threadIndex                          current philosopher thread

Notes
Returns         None
*****************************************************************************************************/
void *philosopherActivity(void *threadIndex)
{
    int *thread = (int*) threadIndex;          // pass integer type argument from thread routine back to child thread

    printf("Philosopher #%d: start thinking..\n", *thread);
    thinking();
    printf("Philosopher #%d: end thinking...\n", *thread);
    /* WHILE LOOP: loop the process of picking up the chopsticks and sleeping quickly in between
                   eat, then increment next thread index by 1 until all threads finished */
    while (pickUpChopsticks(*thread))
        sleep(.90);  // sleep for less than 1 secs in between each print

    printf("Philosopher #%d: start eating..\n", *thread);
    eating();
    printf("Philosopher #%d: end eating...\n", *thread);

    putDownChopsticks(*thread);

    pthread_exit(NULL);
}
