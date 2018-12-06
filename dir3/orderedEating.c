#include <stdio.h>//basic functionality of C Program
#include <stdlib.h>//exit(..),srandom(..),
#include <string.h>//strerror(..)
#include <errno.h>//error handling
#include <unistd.h>//usleep(..)
#include <time.h>//srandom(..)
#include <pthread.h>//POSIX thread, POSIX Mutex Usage & Operations


// Globals
int nthreads;                                   // global num of arguments passed as first parameter in main()
pthread_mutex_t chopstick = PTHREAD_MUTEX_INITIALIZER;         // mutex lock
pthread_cond_t v = PTHREAD_COND_INITIALIZER;                   // conditional variable
pthread_condattr_t cattr;                                      // conditional attr
static int nextIndex=0;                                        // set nextIndex to 0


// Prototype Declarations
void thinking();
void pickUpChopsticks(int threadIndex);
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
     - acts as main processing thread

Returns:
     0                                          function returns successfully
     -1                                         function returns with error status
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
                 accessing chopstick resource */
    for (ndxThread = 0; ndxThread < nthreads; ++ndxThread) {
        /* IF-CONDITION: check status of pthread_mutex_init and print an associated error message if
                         mutex init failed */
        if (error = pthread_mutex_init(&chopstick, NULL)) {
	    fprintf(stderr, "Failed to initialize mutex chopstick: %s\n", strerror(error));
	    return -1;
	}
    }


    /* FOR LOOP: initialize the conditional variable for ordered eating
    for (ndxThread = 0; ndxThread < nthreads; ++ndxThread) {
        // IF-CONDITION: check status of pthread_cond_t and print an associated error message if
                         conditional variable init failed *
        if (error = pthread_cond_init(&v, &cattr)) {
	    fprintf(stderr, "Failed to initialize conditional v : %s\n", strerror(error));
	    return -1;
	}
    }
    */

    /* FOR LOOP: for each philosopher thread, create them using the arguments passed in from
                 threading routine */
    for (ndxThread = 0 ; ndxThread < nthreads ; ++ndxThread) {
        int *args = (int*)malloc(sizeof(int));
        *args = ndxThread/* + 1*/;
        // Initialize thread attributes for each thread
        pthread_attr_t attr;
        pthread_attr_init(&attr);
	/* IF-CONDITION: check status of pthread_create and print an associated error message if
                         creation failed */
        if (error = pthread_create(&tids[ndxThread], &attr, philosopherActivity,  (void *) args)) {
	    fprintf(stderr, "Failed to create thread: %s\n", strerror(error));
	    return -1;
        }
    }


    /*FOR LOOP: for each philosopher thread, join the created threads by passing each philosopher thread
                to pthread_join(...) */
    for (ndxThread = 0 ; ndxThread < nthreads ; ++ndxThread) {
        /* IF-CONDITION: check status of pthread_join abd print an associated error message if
                         join failed */
        if (error = pthread_join(tids[ndxThread], NULL)) {
	    fprintf(stderr, "Failed to join threads: %s\n", strerror(error));
	    return -1;
	}
    }

    pthread_mutex_destroy(&chopstick);    // destroy mutex
    //pthread_cond_destroy(&v);             // destroy the conditional variable

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

    return;
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

     - the print statements help see what is going on more clearer

Returns         None
****************************************************************************************************/
void pickUpChopsticks(int threadIndex)
{
    int c1 = threadIndex; int c2 = threadIndex + 1;
    int error;
    if(c2 > nthreads)
        c2 = 0;
    if (error = pthread_mutex_lock(&chopstick))
        fprintf(stderr, "P %d fails to pick up chopstick: %s\n", threadIndex, strerror(error));
    printf("P %d now picking up their chopsticks\n\n", threadIndex);    // help to add context

    return;
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

Notes
     - the print statements help see what is going on more clearer

Returns         None
****************************************************************************************************/
void putDownChopsticks(int threadIndex)
{
    int c1 = threadIndex; int c2 = threadIndex + 1;
    int error;
    if(c2 > nthreads)
        c2 = 0;
    if (error = pthread_mutex_unlock(&chopstick))
        fprintf(stderr, "P %d fails to release chopstick: %s\n", threadIndex, strerror(error));

    printf("P %d now releasing their chopsticks\n\n", threadIndex);     // help to add context
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
    int *thread = (int *) threadIndex;
    printf("Philosopher #%d: start thinking..\n", *thread);
    thinking();
    printf("Philosopher #%d: end thinking...\n", *thread);

    /* Test conditional variable
    WHILE LOOP: while next thread index NOT equal to current thread index, check current thread
                index to see if matches next thread index then break out to process it if it does */
    while (nextIndex != *thread) {
        // if thread index equals next thread index, then break out and continue executing
        if (*thread == nextIndex)
            break;
    }
    pickUpChopsticks(*thread);
    printf("Philosopher #%d: start eating..\n", *thread);
    eating();
    printf("Philosopher #%d: end eating...\n", *thread);


    putDownChopsticks(*thread);
    nextIndex = nextIndex + 1;               // increment value of nextIndex post-eating

    pthread_exit(NULL);
}
