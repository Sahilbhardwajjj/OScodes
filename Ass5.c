/*
Name-Ayush Karanjkhele
RollNo.-I3104

ASSIGNMENT 5:-
Problem Statement:-
Implement the C program for Deadlock Avoidance Algorithm: Bankers 
Algorithm.
*/
//code 
#include <stdio.h>
#include <stdlib.h>

/* Structure for each process */
struct process {
    int all[6], max[6], need[6], finished, request[6];
} p[10];

/* Global variables */
int avail[6], sseq[10], ss = 0, check1 = 0, check2 = 0;
int n, pid, work[6];
int nor, nori;

/* Function prototypes */
int safeseq(void);

/* ----- helper: wait so output remains on screen ----- */
void wait_for_enter(void) {
    int c;
    printf("\n\nPress Enter to continue...");
    /* Flush any leftover chars (like the '\n' from previous scanf) */
    do { c = getchar(); } while (c != '\n' && c != EOF);
    /* Now wait for a real Enter from user */
    getchar();
}

int main(void)
{
    int ch, i = 0, j = 0, k, pid, ch1;
    int violationcheck = 0, waitcheck = 0;

    do {
        /* Do NOT clear the screen on Ubuntu; keep output visible */
        printf("\n\n\t 1. Input");
        printf("\n\n\t 2. New Request");
        printf("\n\n\t 3. Safe State or Not");
        printf("\n\n\t 4. Print");
        printf("\n\n\t 5. Exit");
        printf("\n\n\t Enter your choice : ");

        if (scanf("%d", &ch) != 1) { /* basic guard */
            printf("\nInvalid input. Exiting.\n");
            return 0;
        }

        switch (ch) {
            case 1:
                printf("\n\n\t Enter number of processes : ");
                scanf("%d", &n);

                printf("\n\n\t Enter the Number of Resources : ");
                scanf("%d", &nor);

                printf("\n\n\t Enter the Available Resources : ");
                for (k = 0; k < nor; k++) {
                    printf("\n\n\t For Resource type %d : ", k);
                    scanf("%d", &avail[k]);
                }

                /* Initialize all fields */
                for (i = 0; i < n; i++) {
                    for (k = 0; k < nor; k++) {
                        p[i].max[k] = 0;
                        p[i].all[k] = 0;
                        p[i].need[k] = 0;
                        p[i].request[k] = 0;
                    }
                    p[i].finished = 0;
                }

                /* Read Max/Alloc, compute Need, update Avail = total - allocated */
                for (i = 0; i < n; i++) {
                    printf("\n\n\t Enter Max and Allocated resources for P%d : ", i);
                    for (j = 0; j < nor; j++) {
                        printf("\n\n\t Enter the Max of resource %d : ", j);
                        scanf("%d", &p[i].max[j]);
                        printf("\n\n\t Allocation of resource %d    : ", j);
                        scanf("%d", &p[i].all[j]);
                        if (p[i].all[j] > p[i].max[j]) {
                            printf("\n\n\t Allocation should be <= Max, please re-enter this resource.");
                            j--; /* repeat this j */
                        } else {
                            p[i].need[j] = p[i].max[j] - p[i].all[j];
                            avail[j] -= p[i].all[j];
                        }
                    }
                }
                break;

            case 2:
                violationcheck = 0;
                waitcheck = 0;

                printf("\n\n\t Requesting process id (0..%d): ", n-1);
                scanf("%d", &pid);

                if (pid < 0 || pid >= n) {
                    printf("\n\n\t Invalid PID.");
                    break;
                }

                for (j = 0; j < nor; j++) {
                    printf("\n\n\t Number of Request for resource %d : ", j);
                    scanf("%d", &p[pid].request[j]);
                    if (p[pid].request[j] > p[pid].need[j])
                        violationcheck = 1;     /* exceeds maximum claim */
                    if (p[pid].request[j] > avail[j])
                        waitcheck = 1;          /* not enough currently available */
                }

                if (violationcheck == 1) {
                    printf("\n\n\t The Process Exceeds its Max Need: Terminated");
                } else if (waitcheck == 1) {
                    printf("\n\n\t Lack of Resources : Process State – Wait");
                } else {
                    /* Tentatively allocate and test safety */
                    for (j = 0; j < nor; j++) {
                        avail[j]    -= p[pid].request[j];
                        p[pid].all[j]  += p[pid].request[j];
                        p[pid].need[j] -= p[pid].request[j];
                    }
                    ch1 = safeseq();
                    if (ch1 == 0) {
                        printf("\n\n\t Granting leads to Unsafe state: Request Denied");
                        /* rollback */
                        for (j = 0; j < nor; j++) {
                            avail[j]    += p[pid].request[j];
                            p[pid].all[j]  -= p[pid].request[j];
                            p[pid].need[j] += p[pid].request[j];
                        }
                    } else {
                        printf("\n\n\t Request Committed ");
                    }
                }
                break;

            case 3:
                if (safeseq() == 1)
                    printf("\n\n\t The System is in SAFE state ");
                else
                    printf("\n\n\t The System is NOT in safe state ");
                break;

            case 4:
                printf("\n\n\t Number of processes : %d", n);
                printf("\n\n\t Number of Resources : %d", nor);

                printf("\n\n\t Pid \t   Max \t   Allocated \t Need ");
                for (i = 0; i < n; i++) {
                    int r;
                    printf("\n\n\t  P%d : ", i);

                    for (r = 0; r < nor; r++) printf(" %d ", p[i].max[r]);
                    printf("\t");
                    for (r = 0; r < nor; r++) printf(" %d ", p[i].all[r]);
                    printf("\t");
                    for (r = 0; r < nor; r++) printf(" %d ", p[i].need[r]);
                }

                printf("\n\n\t Available : ");
                for (i = 0; i < nor; i++) printf(" %d ", avail[i]);
                break;

            case 5:
                printf("\n\nExiting...\n");
                return 0;

            default:
                printf("\n\n\t Invalid choice.");
        }

        /* Keep output visible on Ubuntu terminal */
        wait_for_enter();

    } while (ch != 5);

    return 0;
}

/* ---------- SAFE SEQUENCE FUNCTION ---------- */
int safeseq(void)
{
    int i, j, t;
    ss = 0;

    /* Initialize work = avail */
    for (j = 0; j < nor; j++)
        work[j] = avail[j];

    for (j = 0; j < n; j++)
        p[j].finished = 0;

    /* Try to find n finishable processes (Banker's safety check) */
    for (t = 0; t < n; t++) {
        for (j = 0; j < n; j++) {
            if (p[j].finished == 0) {
                check1 = 0;
                for (i = 0; i < nor; i++)
                    if (p[j].need[i] <= work[i])
                        check1++;
                if (check1 == nor) {
                    for (i = 0; i < nor; i++)
                        work[i] += p[j].all[i];
                    p[j].finished = 1;
                    sseq[ss++] = j;
                }
            }
        }
    }

    check2 = 0;
    for (i = 0; i < n; i++)
        if (p[i].finished == 1)
            check2++;

    if (check2 >= n) {
        printf("\n\n\t The system is in SAFE state\n\t Safe sequence : ");
        for (i = 0; i < n; i++)
            printf("P%d%s", sseq[i], (i < n - 1) ? " -> " : "");
        return 1;
    } else {
        printf("\n\n\t The system is NOT in safe state");
        return 0;
    }
}
// Output
/*
ayush@Legion:~/Ayush_I3104$ gcc Ass5.c 
ayush@Legion:~/Ayush_I3104$ ./a.out


	 1. Input

	 2. New Request

	 3. Safe State or Not

	 4. Print

	 5. Exit

	 Enter your choice : 1


	 Enter number of processes : 3


	 Enter the Number of Resources : 3


	 Enter the Available Resources : 

	 For Resource type 0 : 10


	 For Resource type 1 : 5


	 For Resource type 2 : 7


	 Enter Max and Allocated resources for P0 : 

	 Enter the Max of resource 0 : 7


	 Allocation of resource 0    : 0


	 Enter the Max of resource 1 : 5


	 Allocation of resource 1    : 1


	 Enter the Max of resource 2 : 3


	 Allocation of resource 2    : 0


	 Enter Max and Allocated resources for P1 : 

	 Enter the Max of resource 0 : 3


	 Allocation of resource 0    : 2


	 Enter the Max of resource 1 : 2


	 Allocation of resource 1    : 0


	 Enter the Max of resource 2 : 2


	 Allocation of resource 2    : 0


	 Enter Max and Allocated resources for P2 : 

	 Enter the Max of resource 0 : 9


	 Allocation of resource 0    : 3


	 Enter the Max of resource 1 : 0


	 Allocation of resource 1    : 0


	 Enter the Max of resource 2 : 3


	 Allocation of resource 2    : 3


Press Enter to continue...


	 1. Input

	 2. New Request

	 3. Safe State or Not

	 4. Print

	 5. Exit

	 Enter your choice : 4


	 Number of processes : 3

	 Number of Resources : 3

	 Pid 	   Max 	   Allocated 	 Need 

	  P0 :  7  5  3 	 0  1  0 	 7  4  3 

	  P1 :  3  2  2 	 2  0  0 	 1  2  2 

	  P2 :  9  0  3 	 3  0  3 	 6  0  0 

	 Available :  5  4  4 

Press Enter to continue...


	 1. Input

	 2. New Request

	 3. Safe State or Not

	 4. Print

	 5. Exit

	 Enter your choice : 3


	 The system is in SAFE state
	 Safe sequence : P1 -> P2 -> P0

	 The System is in SAFE state 

Press Enter to continue...


	 1. Input

	 2. New Request

	 3. Safe State or Not

	 4. Print

	 5. Exit

	 Enter your choice : 2


	 Requesting process id (0..2): 1


	 Number of Request for resource 0 : 0


	 Number of Request for resource 1 : 0


	 Number of Request for resource 2 : 1


	 The system is in SAFE state
	 Safe sequence : P1 -> P2 -> P0

	 Request Committed 

Press Enter to continue...


	 1. Input

	 2. New Request

	 3. Safe State or Not

	 4. Print

	 5. Exit

	 Enter your choice : 5


Exiting...
ayush@Legion:~/Ayush_I3104$
*/
