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
*/












this c program demonstrates the *producer-consumer problem* using *threads* and *semaphores*.
it simulates multiple producer threads and a consumer thread that share a common circular buffer.
synchronization between them is handled by *counting semaphores* and a *binary semaphore (mutex)* to avoid race conditions.

important terms
producer – thread that generates items and puts them into the shared buffer.
consumer – thread that takes items from the shared buffer.
buffer – shared circular array of fixed size (buffer_size = 20).
semaphore – synchronization tool that controls access to shared resources.
mutex – binary semaphore used to provide mutual exclusion (only one thread accesses critical section at a time).
sem_wait() – decreases semaphore value; if zero, thread waits (blocks).
sem_post() – increases semaphore value; signals another waiting thread.

main()

•⁠  ⁠defines shared buffer and semaphores.
•⁠  ⁠initializes semaphores:
  • sh.empty = buffer_size (all slots empty initially)
  • sh.full = 0 (no filled slots initially)
  • mutex = 1 (acts as a lock).
•⁠  ⁠creates two producer threads and one consumer thread using pthread_create().
•⁠  ⁠joins all threads (though they run infinitely for demo).

producer()

•⁠  ⁠continuously generates items (simple increasing integers).
•⁠  ⁠waits (sem_wait) for an empty slot before producing.
•⁠  ⁠locks the buffer using sem_wait(&mutex).
•⁠  ⁠inserts item into buffer at index ‘in’.
•⁠  ⁠increments ‘in’ circularly: in = (in + 1) % buffer_size.
•⁠  ⁠prints thread id and produced item.
•⁠  ⁠unlocks mutex using sem_post(&mutex).
•⁠  ⁠signals that one more slot is full using sem_post(&sh.full).
•⁠  ⁠sleeps for 2 seconds for readable output.

consumer()

•⁠  ⁠continuously consumes items from the shared buffer.
•⁠  ⁠waits (sem_wait) for a filled slot before consuming.
•⁠  ⁠locks the buffer using sem_wait(&mutex).
•⁠  ⁠reads item from buffer at index ‘out’.
•⁠  ⁠increments ‘out’ circularly: out = (out + 1) % buffer_size.
•⁠  ⁠prints thread id and consumed item.
•⁠  ⁠unlocks mutex using sem_post(&mutex).
•⁠  ⁠signals that one more slot is empty using sem_post(&sh.empty).
•⁠  ⁠sleeps for 2 seconds for readability.

execution flow summary

1.⁠ ⁠producer waits if buffer is full (no empty slots).
2.⁠ ⁠consumer waits if buffer is empty (no full slots).
3.⁠ ⁠mutex ensures only one thread (either producer or consumer) modifies the buffer at any moment.
4.⁠ ⁠semaphores full and empty keep track of buffer status.

key synchronization logic
•⁠  ⁠empty semaphore prevents producer overflow.
•⁠  ⁠full semaphore prevents consumer underflow.
•⁠  ⁠mutex avoids simultaneous read/write conflicts.

output (example)
producer thread id: 1234, produced item: 0
producer thread id: 1235, produced item: 1
consumer thread id: 1236, consumed item: 0
producer thread id: 1234, produced item: 2
consumer thread id: 1236, consumed item: 1

the output continuously alternates between producers and consumer, showing how threads cooperate safely using semaphores.

in short – this program models the classic *producer-consumer synchronization 
problem* where semaphores control buffer access and ensure smooth coordination
    between producers and consumers without race conditions or deadlocks.
