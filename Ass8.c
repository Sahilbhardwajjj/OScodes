/*
Name-Ayush Karanjkhele
RollNo.-I3104

ASSIGNMENT 8:-

Problem Statement:-
Implement the C program for Disk Scheduling Algorithms: SSTF, SCAN, C-Look considering the initial head position moving away from the spindle.

*/

//Code
#include <stdio.h>
#include <stdlib.h>

void run_SSTF(void);
void run_SCAN(void);
void run_CLOOK(void);

/* Utility: bubble sort (small N friendly, simple) */
static void sort(int a[], int n) {
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (a[j] > a[j + 1]) {
                int t = a[j];
                a[j] = a[j + 1];
                a[j + 1] = t;
            }
        }
    }
}

int main(void) {
    while (1) {
        int ch;
        printf("\n=========== DISK SCHEDULING ===========\n");
        printf("1) SSTF\n");
        printf("2) SCAN\n");
        printf("3) C-LOOK\n");
        printf("4) EXIT\n");
        printf("Enter choice: ");
        if (scanf("%d", &ch) != 1) return 0;

        switch (ch) {
            case 1: run_SSTF();  break;
            case 2: run_SCAN();  break;
            case 3: run_CLOOK(); break;
            case 4: return 0;
            default: printf("Invalid choice.\n"); break;
        }
    }
}

/* ---------------------- SSTF ---------------------- */
void run_SSTF(void) {
    int n, initial;
    int RQ[100];
    int used[100] = {0};
    long long total = 0;

    printf("\nSSTF\nEnter number of requests: ");
    scanf("%d", &n);
    if (n < 1 || n > 100) { printf("Bad n.\n"); return; }

    printf("Enter %d requests: ", n);
    for (int i = 0; i < n; ++i) scanf("%d", &RQ[i]);

    printf("Enter initial head position: ");
    scanf("%d", &initial);

    int head = initial;
    printf("Service order: %d", head);

    for (int served = 0; served < n; ++served) {
        int best = -1;
        int bestDist = 1e9;
        for (int i = 0; i < n; ++i) {
            if (!used[i]) {
                int d = RQ[i] - head;
                if (d < 0) d = -d;
                if (d < bestDist) {
                    bestDist = d;
                    best = i;
                }
            }
        }
        total += bestDist;
        head = RQ[best];
        used[best] = 1;
        printf(" -> %d", head);
    }
    printf("\nTotal head movement: %lld\n", total);
}

/* ---------------------- SCAN ---------------------- */
void run_SCAN(void) {
    int n, initial, size, move;
    int RQ[100];

    printf("\nSCAN\nEnter number of requests: ");
    scanf("%d", &n);
    if (n < 1 || n > 100) { printf("Bad n.\n"); return; }

    printf("Enter %d requests: ", n);
    for (int i = 0; i < n; ++i) scanf("%d", &RQ[i]);

    printf("Enter initial head position: ");
    scanf("%d", &initial);

    printf("Enter total disk size (number of cylinders): ");
    scanf("%d", &size);

    printf("Head direction (1 = towards higher, 0 = towards lower): ");
    scanf("%d", &move);

    sort(RQ, n);

    int head = initial;
    long long total = 0;
    printf("Service order: %d", head);

    /* find first index with RQ[i] >= head */
    int idx = 0;
    while (idx < n && RQ[idx] < head) idx++;

    if (move == 1) {
        /* go up */
        for (int i = idx; i < n; ++i) {
            total += (RQ[i] >= head) ? (RQ[i] - head) : (head - RQ[i]);
            head = RQ[i];
            printf(" -> %d", head);
        }
        /* go to end (size-1), then come back to the last lower request if any */
        if (idx > 0) {
            total += (size - 1 - head);
            head = size - 1;
            printf(" -> %d", head);
            total += (head - RQ[idx - 1]);
            head = RQ[idx - 1];
            printf(" -> %d", head);
            for (int i = idx - 2; i >= 0; --i) {
                total += (head - RQ[i]);
                head = RQ[i];
                printf(" -> %d", head);
            }
        }
    } else {
        /* go down */
        for (int i = idx - 1; i >= 0; --i) {
            total += (head >= RQ[i]) ? (head - RQ[i]) : (RQ[i] - head);
            head = RQ[i];
            printf(" -> %d", head);
        }
        /* go to start (0), then go up to higher requests if any */
        if (idx < n) {
            total += head; /* distance to 0 */
            head = 0;
            printf(" -> %d", head);
            total += (RQ[idx] - head);
            head = RQ[idx];
            printf(" -> %d", head);
            for (int i = idx + 1; i < n; ++i) {
                total += (RQ[i] - head);
                head = RQ[i];
                printf(" -> %d", head);
            }
        }
    }

    printf("\nTotal head movement: %lld\n", total);
}

/* ---------------------- C-LOOK ---------------------- */
void run_CLOOK(void) {
    int n, initial, size, move;
    int RQ[100];

    printf("\nC-LOOK\nEnter number of requests: ");
    scanf("%d", &n);
    if (n < 1 || n > 100) { printf("Bad n.\n"); return; }

    printf("Enter %d requests: ", n);
    for (int i = 0; i < n; ++i) scanf("%d", &RQ[i]);

    printf("Enter initial head position: ");
    scanf("%d", &initial);

    printf("Enter total disk size (ignored by C-LOOK logic but kept for parity): ");
    scanf("%d", &size);

    printf("Head direction (1 = towards higher, 0 = towards lower): ");
    scanf("%d", &move);

    sort(RQ, n);

    int head = initial;
    long long total = 0;
    printf("Service order: %d", head);

    int idx = 0;
    while (idx < n && RQ[idx] < head) idx++;

    if (move == 1) {
        /* go up to highest request */
        for (int i = idx; i < n; ++i) {
            total += (RQ[i] - head >= 0) ? (RQ[i] - head) : (head - RQ[i]);
            head = RQ[i];
            printf(" -> %d", head);
        }
        if (idx > 0) {
            /* jump to lowest request (circular) */
            total += (RQ[n - 1] >= RQ[0]) ? (RQ[n - 1] - RQ[0]) : (RQ[0] - RQ[n - 1]);
            head = RQ[0];
            printf(" -> %d", head);
            for (int i = 1; i < idx; ++i) {
                total += (RQ[i] - head);
                head = RQ[i];
                printf(" -> %d", head);
            }
        }
    } else {
        /* go down to lowest request below head */
        for (int i = idx - 1; i >= 0; --i) {
            total += (head - RQ[i] >= 0) ? (head - RQ[i]) : (RQ[i] - head);
            head = RQ[i];
            printf(" -> %d", head);
        }
        if (idx < n) {
            /* jump to highest request (circular) */
            total += (RQ[n - 1] >= RQ[0]) ? (RQ[n - 1] - RQ[0]) : (RQ[0] - RQ[n - 1]);
            head = RQ[n - 1];
            printf(" -> %d", head);
            for (int i = n - 2; i >= idx; --i) {
                total += (head - RQ[i]);
                head = RQ[i];
                printf(" -> %d", head);
            }
        }
    }

    printf("\nTotal head movement: %lld\n", total);
}
//Output
/*
ayush@Legion:~/Ayush_I3104$ gcc Ass8.c -o disk && ./disk

=========== DISK SCHEDULING ===========
1) SSTF
2) SCAN
3) C-LOOK
4) EXIT
Enter choice: 1

SSTF
Enter number of requests: 8
Enter 8 requests: 84 
153
32
128
10
133
61
69
Enter initial head position: 68
Service order: 68 -> 69 -> 61 -> 84 -> 128 -> 133 -> 153 -> 32 -> 10
Total head movement: 244

=========== DISK SCHEDULING ===========
1) SSTF
2) SCAN
3) C-LOOK
4) EXIT
Enter choice: 2

SCAN
Enter number of requests: 8
Enter 8 requests: 153
84
32
128
10
133
61
69
Enter initial head position: 68
Enter total disk size (number of cylinders): 640
Head direction (1 = towards higher, 0 = towards lower): 0
Service order: 68 -> 61 -> 32 -> 10 -> 0 -> 69 -> 84 -> 128 -> 133 -> 153
Total head movement: 221
=========== DISK SCHEDULING ===========
1) SSTF
2) SCAN
3) C-LOOK
4) EXIT
Enter choice: 3

C-LOOK
Enter number of requests: 8
Enter 8 requests: 84
153
32
128
10
133
61
69
Enter initial head position: 68
Enter total disk size (ignored by C-LOOK logic but kept for parity): 640
Head direction (1 = towards higher, 0 = towards lower): 1
Service order: 68 -> 69 -> 84 -> 128 -> 133 -> 153 -> 10 -> 32 -> 61
Total head movement: 279

=========== DISK SCHEDULING ===========
1) SSTF
2) SCAN
3) C-LOOK
4) EXIT
Enter choice: 4
ayush@Legion:~/Ayush_I3104$ 

*/
