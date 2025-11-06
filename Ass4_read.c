#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/types.h>

/* Function prototypes for reader and writer threads */
void *reader(void *argp);
void *writer(void *argp);

/* Shared buffer (single slot) */
int buf;

/* Writer: input an item from user */
int getbuff(){
    int temp;
    printf("\nEnter the item in buffer :- ");   // prompt for input
    scanf("%d",&temp);                          // read integer from stdin
    return temp;                                // return entered value
}

/* Reader: print the item from buffer */
void readbuff(int buf){
    printf("\nItem READ from buffer : %d\n",buf);
}

/* Synchronization variables */
/* mutex1 protects read_count updates (number of readers) */
/* wrt ensures mutual exclusion for writers (and blocks readers when a writer writes) */
pthread_mutex_t mutex1=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wrt=PTHREAD_MUTEX_INITIALIZER;

int read_count=0;   // number of active readers
int flag=0;         // 0 = buffer empty, 1 = buffer contains new data

/* ---------- Writer thread function ---------- */
void *writer(void *argp)
{
    while(1)
    {
        pthread_mutex_lock(&wrt);   // lock wrt to get exclusive access
        if(flag==0)                 // only write if buffer is empty
        {
            buf=getbuff();          // read value from user into buffer
            flag=1;                 // mark buffer as full
        }
        pthread_mutex_unlock(&wrt); // release wrt
    }
}

/* ---------- Reader thread function ---------- */
void *reader(void *argp)
{
    while(1)
    {
        /* Entry section for reader */
        pthread_mutex_lock(&mutex1);   // lock mutex1 before changing read_count
        read_count++;
        if(read_count==1)
        {
            pthread_mutex_lock(&wrt);  // first reader locks wrt (blocks writers)
        }
        pthread_mutex_unlock(&mutex1); // release mutex1

        /* Critical section for reader */
        if(flag==1)                    // if buffer has data
        {
            readbuff(buf);             // read (print) the buffer content
            sleep(2);                  // simulate read delay
            flag=0;                    // mark buffer empty again
        }

        /* Exit section for reader */
        pthread_mutex_lock(&mutex1);   // lock mutex1 before changing read_count
        read_count--;
        if(read_count==0)
        {
            pthread_mutex_unlock(&wrt); // last reader unlocks wrt (writers can proceed)
        }
        pthread_mutex_unlock(&mutex1); // release mutex1
    }
}

/* ---------- Main program ---------- */
int main()
{
    pthread_t tid1,tid2,tid3;   // thread handles: 1 writer, 2 readers

    /* create one writer and two reader threads */
    pthread_create(&tid1,NULL,writer,NULL);
    pthread_create(&tid2,NULL,reader,NULL);
    pthread_create(&tid3,NULL,reader,NULL);

    /* wait for threads (they run infinitely, so program runs until killed) */
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);

    return 0;
}
// Output
/*
ayush@Legion:~/Ayush_I3104$ gcc Ass4_read.c -o read -lpthread
ayush@Legion:~/Ayush_I3104$ ./read

Enter the item in buffer :- 6

Item READ from buffer : 6

Item READ from buffer : 6

Enter the item in buffer :- 7

Item READ from buffer : 7

Item READ from buffer : 7

Enter the item in buffer :- 67

Item READ from buffer : 67

Item READ from buffer : 67

Enter the item in buffer :-
*/

| *Topic*                             | *Question*                            | *Short Answer*                                                                                                                                  |
| ------------------------------------- | --------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------- |
| *Concept*                           | What is the Readers–Writers problem?    | A synchronization problem where multiple readers can read at the same time, but only one writer can write — and no reader can read while writing. |
| *Shared Resource*                   | What is the shared resource here?       | The single-slot buffer ⁠ buf ⁠.                                                                                                                     |
| *Goal*                              | What’s the aim of the program?          | To allow multiple readers simultaneously but only one writer exclusively.                                                                         |
| *Threads*                           | How many threads are created?           | One writer and two readers.                                                                                                                       |
| *Mutex Variables*                   | Which mutexes are used?                 | ⁠ mutex1 ⁠ → protects ⁠ read_count ⁠; ⁠ wrt ⁠ → ensures exclusive access for writers.                                                                   |
| *⁠ read_count ⁠ use*                  | What does ⁠ read_count ⁠ represent?       | Number of active reader threads currently reading.                                                                                                |
| *⁠ flag ⁠ use*                        | What is ⁠ flag ⁠ for?                     | Indicates buffer status → ⁠ 0 ⁠ = empty, ⁠ 1 ⁠ = full (new data available).                                                                           |
| *Reader Logic*                      | How does reader ensure synchronization? | First reader locks ⁠ wrt ⁠ to block writers; last reader unlocks ⁠ wrt ⁠ after finishing.                                                             |
| *Writer Logic*                      | What ensures only one writer at a time? | Writer locks ⁠ wrt ⁠ before writing and unlocks after writing.                                                                                      |
| *Entry Section*                     | What happens in reader’s entry section? | ⁠ read_count ⁠ incremented safely using ⁠ mutex1 ⁠; first reader locks ⁠ wrt ⁠.                                                                         |
| *Exit Section*                      | What happens in reader’s exit section?  | ⁠ read_count ⁠ decremented safely; last reader unlocks ⁠ wrt ⁠.                                                                                       |
| *Critical Section*                  | What’s the critical section?            | Reading or writing to the shared buffer ⁠ buf ⁠.                                                                                                    |
| *Mutual Exclusion*                  | How is mutual exclusion achieved?       | Using ⁠ pthread_mutex_lock() ⁠ and ⁠ pthread_mutex_unlock() ⁠.                                                                                        |
| *Fairness*                          | Can readers starve the writer?          | Yes, in this version — continuous readers can keep blocking writer (reader preference).                                                           |
| *Improvement*                       | How to prevent writer starvation?       | Use writer-priority or fair semaphore version (limit continuous reader access).                                                                   |
| *Why ⁠ pthread_mutex_lock ⁠?*         | Purpose of this function?               | Locks the mutex to protect shared variable updates.                                                                                               |
| *Why ⁠ sleep(2) ⁠ in reader?*         | Why added?                              | Simulates read delay and makes output readable.                                                                                                   |
| *Difference from Producer–Consumer* | How is this different?                  | Here, multiple readers can read simultaneously; Producer–Consumer allows only one consumer at a time.                                             |
| *Deadlock Possibility*              | Can deadlock occur here?                | Not if locks are acquired and released in correct order.                                                                                          |
| *Termination*                       | Why does program run indefinitely?      | Infinite loops in threads (⁠ while(1) ⁠); manual termination needed.                                                                                |
| *Key Synchronization Rule*          | Main rule of readers–writers problem?   | No reader should read while a writer writes, and only one writer at a time.       
