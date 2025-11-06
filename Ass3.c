/*
Name-Ayush Karanjkhele
RollNo.-I3104

ASSIGNMENT 3:-
Problem Statement:-
Implement the C program for CPU Scheduling Algorithms: Shortest 
Job First (Preemptive) and Round Robin with different arrival time.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 20
#define QSIZE (MAX * 10)

// Structure to hold process control block information
typedef struct {
    char name[16];
    int AT;     // Arrival Time
    int BT;     // Original Burst Time (unchanged)
    int rem;    // Remaining Burst Time (used by algorithms)
    int WT;     // Waiting Time
    int TAT;    // Turnaround Time
    int PID;    // Process ID (index)
} process;

// Structure for Gantt chart representation
typedef struct {
    char name[16];
    int start;
    int end;
} gantt_event;

/* prototypes */
void get_PCB(process p[], int *n);
void disp_table(process p[], int n);
float cal_avgwt(process p[], int n);
float cal_avgtat(process p[], int n);

void SJF_Preemptive(process p[], int n);
void RoundRobin(process p[], int n, int tq);

/**
 * @brief Displays the main menu options.
 */
void menu() {
    printf("\n\t**** CPU SCHEDULING MENU ****\n");
    printf("1. SJF (Preemptive - SRTF)\n");
    printf("2. Round Robin\n");
    printf("3. Exit\n");
    printf("Enter choice: ");
}

/**
 * @brief Main function to run the simulator.
 */
int main() {
    process P[MAX];
    int n, ch, tq;

    // Get input processes once
    get_PCB(P, &n);

    // Keep the main loop running until the user exits
    while (1) {
        menu();
        if (scanf("%d", &ch) != 1) {
            // Handle non-integer input
            break; 
        }

        // Create a copy of the processes for each run since the scheduling functions modify 'rem', 'WT', and 'TAT'
        process P_copy[MAX];
        memcpy(P_copy, P, sizeof(process) * n);

        if (ch == 1) {
            SJF_Preemptive(P_copy, n);
            disp_table(P_copy, n);
            printf("\nAverage WT : %.2f\n", cal_avgwt(P_copy, n));
            printf("Average TAT: %.2f\n", cal_avgtat(P_copy, n));
        } else if (ch == 2) {
            printf("Enter Time Quantum: ");
            if (scanf("%d", &tq) != 1) break;
            if (tq <= 0) {
                printf("Time quantum must be >= 1\n");
                continue;
            }
            RoundRobin(P_copy, n, tq);
            disp_table(P_copy, n);
            printf("\nAverage WT : %.2f\n", cal_avgwt(P_copy, n));
            printf("Average TAT: %.2f\n", cal_avgtat(P_copy, n));
        } else {
            break;
        }
    }

    return 0;
}

/* ------------------- INPUT / DISPLAY / AVG ------------------- */

/**
 * @brief Reads process details from the user.
 * @param p Array of process structures.
 * @param n Pointer to store the number of processes.
 */
void get_PCB(process p[], int *n) {
    int i;
    printf("Enter total no of processes: ");
    if (scanf("%d", n) != 1) *n = 0;

    for (i = 0; i < *n; i++) {
        printf("\nProcess %d name (e.g., P%d): ", i + 1, i + 1);
        scanf("%s", p[i].name);
        printf("Arrival Time       : ");
        scanf("%d", &p[i].AT);
        printf("Burst Time         : ");
        scanf("%d", &p[i].BT);
        p[i].rem = p[i].BT;    /* initial remaining time equals BT */
        p[i].WT = p[i].TAT = 0;
        p[i].PID = i;
    }
}

/**
 * @brief Displays the final result table including WT and TAT.
 * @param p Array of process structures.
 * @param n Number of processes.
 */
void disp_table(process p[], int n) {
    int i;
    printf("\n\n P_NAME \t AT \t BT \t WT \t TAT\n");
    for (i = 0; i < n; i++) {
        printf(" %-8s \t %d \t %d \t %d \t %d\n",
               p[i].name, p[i].AT, p[i].BT, p[i].WT, p[i].TAT);
    }
}

/**
 * @brief Calculates the average Waiting Time.
 * @param p Array of process structures.
 * @param n Number of processes.
 * @return The average waiting time.
 */
float cal_avgwt(process p[], int n) {
    float s = 0.0f;
    int i;
    for (i = 0; i < n; i++) s += p[i].WT;
    return s / n;
}

/**
 * @brief Calculates the average Turnaround Time.
 * @param p Array of process structures.
 * @param n Number of processes.
 * @return The average turnaround time.
 */
float cal_avgtat(process p[], int n) {
    float s = 0.0f;
    int i;
    for (i = 0; i < n; i++) s += p[i].TAT;
    return s / n;
}

/* ------------------- SJF PREEMPTIVE (SRTF) ------------------- */

/**
 * @brief Implements Shortest Remaining Time First (SRTF) scheduling.
 * @param p Array of process structures.
 * @param n Number of processes.
 */
void SJF_Preemptive(process p[], int n) {
    int i, completed = 0, time = 0;
    int min_idx;
    int prev_idx = -1;
    gantt_event events[1000];
    int evcount = 0;

    /* Reset remaining times and stats (already done in main, but good to ensure) */
    for (i = 0; i < n; i++) {
        p[i].rem = p[i].BT;
        p[i].WT = p[i].TAT = 0;
    }

    /* Start time at the earliest arrival of any process */
    int earliest = p[0].AT;
    for (i = 1; i < n; i++) if (p[i].AT < earliest) earliest = p[i].AT;
    time = earliest;

    while (completed < n) {
        /* Find ready process with smallest remaining time (SRTF logic) */
        min_idx = -1;
        for (i = 0; i < n; i++) {
            if (p[i].rem > 0 && p[i].AT <= time) {
                if (min_idx == -1 || p[i].rem < p[min_idx].rem ||
                    (p[i].rem == p[min_idx].rem && p[i].AT < p[min_idx].AT)) {
                    min_idx = i;
                }
            }
        }

        if (min_idx == -1) {
            /* CPU idle -> jump to next arrival; record IDLE event */
            int next_arr = -1;
            for (i = 0; i < n; i++) {
                // Find the arrival time of the next incomplete process
                if (p[i].rem > 0) {
                    if (next_arr == -1 || (p[i].AT > time && p[i].AT < next_arr) || next_arr == -1) {
                        next_arr = p[i].AT;
                    }
                }
            }
            
            // If there's an arrival in the future, jump the clock
            if (next_arr != -1 && next_arr > time) {
                if (evcount > 0 && strcmp(events[evcount - 1].name, "IDLE") == 0) {
                    events[evcount - 1].end = next_arr;
                } else {
                    strcpy(events[evcount].name, "IDLE");
                    events[evcount].start = time;
                    events[evcount].end = next_arr;
                    evcount++;
                }
                time = next_arr;
            } else {
                // Should only happen if all processes are complete
                break; 
            }
            continue;
        }

        /* Check for preemption or continuous run */
        if (prev_idx != min_idx) {
            /* new event or context switch */
            strcpy(events[evcount].name, p[min_idx].name);
            events[evcount].start = time;
            events[evcount].end = time; /* will extend */
            evcount++;
        }
        
        /* run one unit (preemptive step) */
        p[min_idx].rem--;
        time++;
        events[evcount - 1].end = time;

        if (p[min_idx].rem == 0) {
            completed++;
            // Calculate metrics only on completion
            p[min_idx].TAT = time - p[min_idx].AT;
            p[min_idx].WT = p[min_idx].TAT - p[min_idx].BT;
        }
        prev_idx = min_idx;
    }

    /* print Gantt chart */
    printf("\n\nGANTT CHART (SJF Preemptive):\n|");
    for (i = 0; i < evcount; i++) {
        printf(" %s |", events[i].name);
    }
    printf("\n%d", events[0].start);
    for (i = 0; i < evcount; i++) {
        printf("     %d", events[i].end);
    }
    printf("\n");
}

/* ------------------- ROUND ROBIN (CORRECTED) ------------------- */

/**
 * @brief Implements Round Robin scheduling.
 * @param p Array of process structures.
 * @param n Number of processes.
 * @param tq Time quantum.
 */
void RoundRobin(process p[], int n, int tq) {
    int i;
    /* Reset remaining times and stats */
    for (i = 0; i < n; i++) {
        p[i].rem = p[i].BT;
        p[i].WT = p[i].TAT = 0;
    }

    int time = 0, completed = 0;
    int queue[QSIZE];
    int head = 0, tail = 0, countq = 0;
    // in_queue tracks if a process is currently waiting in the queue
    int in_queue[MAX]; 
    memset(in_queue, 0, sizeof(in_queue));
    gantt_event events[1000];
    int evcount = 0;

    /* Find the earliest arrival time */
    int earliest = p[0].AT;
    for (i = 1; i < n; i++) if (p[i].AT < earliest) earliest = p[i].AT;
    time = earliest;

    /* Initial enqueue of all processes arrived by 'time' (which is earliest AT) */
    for (i = 0; i < n; i++) {
        if (p[i].AT <= time && !in_queue[i]) {
            queue[tail] = i; tail = (tail + 1) % QSIZE; countq++; in_queue[i] = 1;
        }
    }

    while (completed < n) {
        if (countq == 0) {
            /* CPU is idle. Find the next arrival time among remaining processes. */
            int next_arr = -1;
            for (i = 0; i < n; i++) {
                if (p[i].rem > 0) {
                    // Find the minimum arrival time of any remaining process after current time
                    if (p[i].AT > time) {
                        if (next_arr == -1 || p[i].AT < next_arr) next_arr = p[i].AT;
                    }
                }
            }
            
            if (next_arr != -1 && next_arr > time) {
                /* Record IDLE event */
                if (evcount > 0 && strcmp(events[evcount - 1].name, "IDLE") == 0) {
                    events[evcount - 1].end = next_arr;
                } else {
                    strcpy(events[evcount].name, "IDLE");
                    events[evcount].start = time;
                    events[evcount].end = next_arr;
                    evcount++;
                }
                time = next_arr;
            } else if (next_arr == -1) {
                /* All processes are complete or no more arrivals are possible */
                break;
            }
            
            /* Enqueue processes that have just arrived at the new 'time' (next_arr) */
            for (i = 0; i < n; i++) {
                if (!in_queue[i] && p[i].rem > 0 && p[i].AT <= time) {
                    queue[tail] = i; tail = (tail + 1) % QSIZE; countq++; in_queue[i] = 1;
                }
            }
            continue;
        }

        /* Dequeue next process to run */
        int idx = queue[head]; head = (head + 1) % QSIZE; countq--; in_queue[idx] = 0;

        /* Start a Gantt event for this run */
        strcpy(events[evcount].name, p[idx].name);
        events[evcount].start = time;
        events[evcount].end = time; 
        evcount++;
        
        /* Determine run length for this turn: minimum of remaining time or time quantum */
        int run = (p[idx].rem < tq) ? p[idx].rem : tq;

        /* Execute unit-by-unit so arrivals inside quantum get enqueued at correct times */
        int t;
        for (t = 0; t < run; t++) {
            p[idx].rem--;
            time++;
            events[evcount - 1].end = time;
            
            /* enqueue newly arrived processes at this exact 'time' */
            for (i = 0; i < n; i++) {
                // Crucial fix: Must use p[i].AT == time to maintain strict FIFO queue order
                if (!in_queue[i] && p[i].rem > 0 && p[i].AT == time) {
                    queue[tail] = i; tail = (tail + 1) % QSIZE; countq++; in_queue[i] = 1;
                }
            }
        }

        if (p[idx].rem == 0) {
            /* Process finished */
            completed++;
            p[idx].TAT = time - p[idx].AT;
            p[idx].WT  = p[idx].TAT - p[idx].BT;
        } else {
            /* Process not finished -> re-enqueue at tail */
            queue[tail] = idx; tail = (tail + 1) % QSIZE; countq++; in_queue[idx] = 1;
        }
    }

    /* Print Gantt chart */
    printf("\n\nGANTT CHART (Round Robin, TQ=%d):\n|", tq);
    for (i = 0; i < evcount; i++) {
        printf(" %s |", events[i].name);
    }
    printf("\n%d", events[0].start);
    for (i = 0; i < evcount; i++) {
        printf("     %d", events[i].end);
    }
    printf("\n");
}
//Output
/*
ayush@Legion:~/Ayush_I3104$ gcc Ass3.c
ayush@Legion:~/Ayush_I3104$ ./a.out
Enter total no of processes: 4

Process 1 name (e.g., P1): p1
Arrival Time       : 0
Burst Time         : 3

Process 2 name (e.g., P2): p2
Arrival Time       : 1
Burst Time         : 6

Process 3 name (e.g., P3): p3
Arrival Time       : 4
Burst Time         : 4

Process 4 name (e.g., P4): p4
Arrival Time       : 6
Burst Time         : 2

	**** CPU SCHEDULING MENU ****
1. SJF (Preemptive - SRTF)
2. Round Robin
3. Exit
Enter choice: 1


GANTT CHART (SJF Preemptive):
| p1 | p2 | p3 | p4 | p2 |
0     3     4     8     10     15


 P_NAME 	 AT 	 BT 	 WT 	 TAT
 p1       	 0 	 3 	 0 	 3
 p2       	 1 	 6 	 8 	 14
 p3       	 4 	 4 	 0 	 4
 p4       	 6 	 2 	 2 	 4

Average WT : 2.50
Average TAT: 6.25

	**** CPU SCHEDULING MENU ****
1. SJF (Preemptive - SRTF)
2. Round Robin
3. Exit
Enter choice: 2
Enter Time Quantum: 2


GANTT CHART (Round Robin, TQ=2):
| p1 | p2 | p1 | p3 | p2 | p4 | p3 | p2 |
0     2     4     5     7     9     11     13     15


 P_NAME 	 AT 	 BT 	 WT 	 TAT
 p1       	 0 	 3 	 2 	 5
 p2       	 1 	 6 	 8 	 14
 p3       	 4 	 4 	 5 	 9
 p4       	 6 	 2 	 3 	 5

Average WT : 4.50
Average TAT: 8.25

	**** CPU SCHEDULING MENU ****
1. SJF (Preemptive - SRTF)
2. Round Robin
3. Exit
Enter choice: 3
ayush@Legion:~/Ayush_I3104$
*/
