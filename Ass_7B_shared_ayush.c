/*
Name-Ayush Kothadia
RollNo.-I3106

ASSIGNMENT 7(B):-
Problem Statement:-
Inter-process Communication using Shared Memory using System V IPC. 
Application to demonstrate: Client and Server Programs in which server 
process creates a shared memory segment and writes the message to the 
shared memory segment. Client process reads the message from the 
shared memory segment and displays it to the screen

*/

//Code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SHM_SIZE 4096  // 4 KB

int main(void) {
    // Create shared memory
    int shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        return 1;
    }
    printf("Shared memory created. ID = %d\n", shmid);

    // Attach once (both parent and child will see it after fork)
    void *addr = shmat(shmid, NULL, 0);
    if (addr == (void *)-1) {
        perror("shmat");
        // remove segment if attach failed
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }
    char *shm_buf = (char *)addr;

    // ---- SERVER (parent before fork): write the message first ----
    char input[SHM_SIZE];
    printf("[Server] Enter the message to write: ");
    fflush(stdout);

    if (!fgets(input, sizeof(input), stdin)) {
        fprintf(stderr, "Failed to read input.\n");
        shmdt(addr);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }
    // Strip trailing newline
    input[strcspn(input, "\n")] = '\0';

    // Write to shared memory
    strncpy(shm_buf, input, SHM_SIZE - 1);
    shm_buf[SHM_SIZE - 1] = '\0';
    printf("[Server] Message written to shared memory.\n");

    // ---- Fork AFTER writing so client always reads valid data ----
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        shmdt(addr);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    if (pid == 0) {
        // ---- CLIENT (child) ----
        printf("\n[Client] Reading from shared memory...\n");
        printf("[Client] Message: %s\n", shm_buf);

        if (shmdt(addr) == -1) {
            perror("shmdt (client)");
            return 1;
        }
        return 0;
    } else {
        // ---- SERVER (parent) cleanup ----
        wait(NULL);  // wait for client to finish

        if (shmdt(addr) == -1) {
            perror("shmdt (server)");
            // continue to try removing segment
        }

        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl IPC_RMID");
            return 1;
        }

        printf("[Server] Shared memory detached and removed. Done.\n");
        return 0;
    }
}

//Output
/*
ayush@Legion:~/Ayush_I3104$ gcc Ass_7B_shared.c
ayush@Legion:~/Ayush_I3104$ ./a.out
Shared memory created. ID = 32807
[Server] Enter the message to write: Hey Im Sending this mssg aka Server
[Server] Message written to shared memory.

[Client] Reading from shared memory...
[Client] Message: Hey Im Sending this mssg aka Server
[Server] Shared memory detached and removed. Done.
ayush@Legion:~/Ayush_I3104$ 
*/
