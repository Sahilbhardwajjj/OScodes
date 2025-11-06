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





























this program demonstrates inter process communication (ipc) using *shared memory* between a parent (server) and child (client) process

important terms:
shmget() – creates a shared memory segment and returns an id
shmat() – attaches the created shared memory segment to the process address space
shmdt() – detaches the shared memory segment from the process
shmctl() – performs operations on shared memory like remove or control
fork() – creates a child process
wait() – makes parent wait until the child process finishes
ipc_private – creates a private shared memory segment used only by related processes
shm_size – defines total memory size (here 4096 bytes)
strncpy() – copies string safely to shared memory buffer


1.⁠ ⁠shared memory is created using shmget() with size 4096 bytes. permissions are set to 0666 so both processes can read and write.
2.⁠ ⁠shmat() attaches the shared memory to the process so it can access it using a pointer (shm_buf).
3.⁠ ⁠parent (server) asks the user to input a message and writes that message into the shared memory.
4.⁠ ⁠after writing, fork() is called to create a child (client) process.
5.⁠ ⁠the child reads the same message directly from shared memory (because both processes share the same memory area).
6.⁠ ⁠child displays the message on the screen, then detaches its shared memory using shmdt().
7.⁠ ⁠parent waits for the child to finish, then detaches the shared memory and removes it completely using shmctl() with ipc_rmid.
short explanation – this program shows communication between parent and child process using shared memory. the server (parent) 
writes a message to shared memory, and the client (child) reads it from the same location. after reading, both detach and the 
shared memory is removed. shared memory allows direct communication without using files or pipes, making it very fast.
