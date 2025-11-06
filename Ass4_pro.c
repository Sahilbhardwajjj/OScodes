/* Name:-Ayush Karanjkhele
Roll No:-I3104
ASSIGNMENT 4(A):-

Problem Statement:-
 Thread synchronization using counting semaphores and mutual 
exclusion using mutex. Application to demonstrate: Producer 
Consumer problem with counting semaphores and
mutex.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>         // sleep()
#include <sys/syscall.h>    // syscall, SYS_gettid
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 20

void *producer(void *arg); // used arg coz pthread_create wants a single void* arg and return void*, we will void arg in local scope
void *consumer(void *arg);

/* Shared structure: circular buffer + counting semaphores */
typedef struct {
    int buff[BUFFER_SIZE];
    sem_t full;   // number of filled slots
    sem_t empty;  // number of empty slots
} shared;

shared sh;           // global shared buffer and semaphores
int in = 0;          // next index for producer to write (0..BUFFER_SIZE-1)
int out = 0;         // next index for consumer to read (0..BUFFER_SIZE-1)
sem_t mutex;         // binary semaphore used as mutual exclusion lock,mutex here is just a binary sema, not actual pthread_mutex_t var
int next_item = 0;   // simple item generator (just increasing int)

/* ---------- main ---------- */
int main()
{
    pthread_t ptid1, ptid2, ctid1;

    /* initialize counting semaphores:
       sh.empty = number of empty slots initially BUFFER_SIZE
       sh.full  = number of filled slots initially 0 */
    sem_init(&sh.empty, 0, BUFFER_SIZE);
    sem_init(&sh.full, 0, 0);

    /* initialize binary semaphore (value 1) to act like a mutex */
    sem_init(&mutex, 0, 1);

    /* create two producers and one consumer */
    pthread_create(&ptid1, NULL, producer, NULL); 
    pthread_create(&ptid2, NULL, producer, NULL);
    pthread_create(&ctid1, NULL, consumer, NULL);

    /* join threads (program will run indefinitely for demo) */
    pthread_join(ptid1, NULL);
    pthread_join(ptid2, NULL);
    pthread_join(ctid1, NULL);

    return 0;
}

/* ---------- producer ---------- */
void *producer(void *arg)
{
    (void)arg; // unused

    while (1)
    {
        /* produce an item (here simply a unique increasing integer) */
        int item = next_item++;

        /* Wait for an empty slot (decrement empty). If no empty slot, block here */
        sem_wait(&sh.empty);

        /* Acquire mutual exclusion to update buffer and index 'in' safely */
        sem_wait(&mutex);

        /* put item into buffer at position 'in' */
        sh.buff[in] = item;
        /* advance 'in' in circular fashion to avoid overflow */
        in = (in + 1) % BUFFER_SIZE;

        /* print thread id and produced item  */
        pid_t tid = (pid_t) syscall(SYS_gettid);
        printf("producer thread id: %d, produced item: %d\n", (int)tid, item);

        /* release mutex */
        sem_post(&mutex);

        /* signal that there is one more full slot */
        sem_post(&sh.full);

        /* small sleep so output is readable */
        sleep(2);
    }

    return NULL;
}

/* ---------- consumer ---------- */
void *consumer(void *arg)
{
    (void)arg; // unused

    while (1)
    {
        /* Wait for a filled slot (decrement full). If no item, block here */
        sem_wait(&sh.full);

        /* Acquire mutual exclusion before reading from buffer */
        sem_wait(&mutex);

        /* take item from buffer at position 'out' */
        int item = sh.buff[out];
        /* advance 'out' circularly */
        out = (out + 1) % BUFFER_SIZE;

        /* print thread id and consumed item */
        pid_t tid = (pid_t) syscall(SYS_gettid);
        printf("consumer thread id: %d, consumed item: %d\n", (int)tid, item);

        /* release mutex */
        sem_post(&mutex);

        /* signal that there is one more empty slot */
        sem_post(&sh.empty);

        /* small sleep so output readable for demo */
        sleep(2);
    }

    return NULL;
}
// Output
/*
ayush@Legion:~/Ayush_I3104$ gcc -pthread  Ass4_pro.c -o produce
ayush@Legion:~/Ayush_I3104$ ./produce
producer thread id: 5287, produced item: 0
producer thread id: 5288, produced item: 1
consumer thread id: 5289, consumed item: 0
producer thread id: 5287, produced item: 2
producer thread id: 5288, produced item: 3
consumer thread id: 5289, consumed item: 1
producer thread id: 5287, produced item: 4
consumer thread id: 5289, consumed item: 2
producer thread id: 5288, produced item: 5
producer thread id: 5287, produced item: 6
consumer thread id: 5289, consumed item: 3
producer thread id: 5288, produced item: 7
consumer thread id: 5289, consumed item: 4
producer thread id: 5288, produced item: 8
producer thread id: 5287, produced item: 9
consumer thread id: 5289, consumed item: 5
producer thread id: 5288, produced item: 10
producer thread id: 5287, produced item: 11
consumer thread id: 5289, consumed item: 6
producer thread id: 5288, produced item: 12
producer thread id: 5287, produced item: 13
consumer thread id: 5289, consumed item: 7
producer thread id: 5287, produced item: 14
producer thread id: 5288, produced item: 15
^C
ayush@Legion:~/Ayush_I3104$
*/
