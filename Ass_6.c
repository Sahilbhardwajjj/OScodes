/*
Name-Ayush Karanjkhele
RollNo.-I3104
ASSIGNMENT 6:-
Problem Statement:-
To implement page replacement algorithms using virtual memory

*/
//code 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_PAGES  200
#define MAX_FRAME  20

/* ---- helpers ---- */
static int in_frames(char frames[], int fsz, char p) {
    for (int i = 0; i < fsz; i++) if (frames[i] == p) return i;
    return -1;
}

static void print_row(char req, char frames[], int used, int hit) {
    printf("\n\t%c\t\t", req);
    for (int i = 0; i < used; i++) {
        printf("%c   ", frames[i] ? frames[i] : '-');
    }
    printf("\t\t%s", hit ? "HIT" : "FAULT");
}

/* ---- FIFO ---- */
void fifo(char pages[], int n, int frameSize) {
    char frames[MAX_FRAME] = {0};
    int used = 0, next = 0, faults = 0;

    printf("\nData Requested\tFrame contents\t    Status\n==============================================");
    for (int i = 0; i < n; i++) {
        char p = pages[i];
        int hit = (in_frames(frames, used, p) != -1);

        if (!hit) {
            faults++;
            if (used < frameSize) {
                frames[used++] = p;
            } else {
                frames[next] = p;
                next = (next + 1) % frameSize;
            }
        }
        print_row(p, frames, used, hit);
    }
    printf("\n\n==============================================\n");
    printf("\nTotal no. of Page Faults: %d\n\n", faults);
}

/* ---- OPTIMAL ---- */
void optimal(char pages[], int n, int frameSize) {
    char frames[MAX_FRAME] = {0};
    int used = 0, faults = 0;

    printf("\nData Requested\tFrame contents\t    Status\n==============================================");
    for (int i = 0; i < n; i++) {
        char p = pages[i];
        int pos = in_frames(frames, used, p);
        int hit = (pos != -1);

        if (!hit) {
            faults++;
            if (used < frameSize) {
                frames[used++] = p;
            } else {
                /* pick the one used farthest in the future (or never) */
                int far_idx = 0, far_dist = -1;
                for (int f = 0; f < used; f++) {
                    int dist = 1000000; /* big = never used again */
                    for (int j = i + 1; j < n; j++) {
                        if (pages[j] == frames[f]) {
                            dist = j - i;
                            break;
                        }
                    }
                    if (dist > far_dist) {
                        far_dist = dist;
                        far_idx = f;
                    }
                }
                frames[far_idx] = p;
            }
        }
        print_row(p, frames, used, hit);
    }
    printf("\n\n==============================================\n");
    printf("\nTotal no. of Page Faults: %d\n\n", faults);
}

/* ---- LRU ---- */
void lru(char pages[], int n, int frameSize) {
    char frames[MAX_FRAME] = {0};
    int last_used[MAX_FRAME] = {0}; /* time of last use */
    int used = 0, faults = 0;

    printf("\nData Requested\tFrame contents\t    Status\n==============================================");
    for (int time = 0; time < n; time++) {
        char p = pages[time];
        int idx = in_frames(frames, used, p);
        int hit = (idx != -1);

        if (hit) {
            last_used[idx] = time;
        } else {
            faults++;
            if (used < frameSize) {
                frames[used] = p;
                last_used[used] = time;
                used++;
            } else {
                /* replace least-recently-used (smallest last_used) */
                int lru_idx = 0, lru_time = last_used[0];
                for (int f = 1; f < used; f++) {
                    if (last_used[f] < lru_time) {
                        lru_time = last_used[f];
                        lru_idx = f;
                    }
                }
                frames[lru_idx] = p;
                last_used[lru_idx] = time;
            }
        }
        print_row(p, frames, used, hit);
    }
    printf("\n\n==============================================\n");
    printf("\nTotal no. of Page Faults: %d\n\n", faults);
}

int main(void) {
    char line[512];
    char pages[MAX_PAGES];
    int n = 0, frameSize, ch;

    printf("Enter the reference string (chars, spaces allowed): ");
    if (!fgets(line, sizeof(line), stdin)) return 0;

    /* keep only non-space, visible chars */
    for (int i = 0; line[i] && n < MAX_PAGES; i++) {
        if (!isspace((unsigned char)line[i])) {
            pages[n++] = line[i];
        }
    }

    printf("Enter the size of the frame: ");
    if (scanf("%d", &frameSize) != 1 || frameSize <= 0 || frameSize > MAX_FRAME) {
        printf("Invalid frame size.\n");
        return 0;
    }

    do {
        printf("\nMENU\n====\n1. FIFO\n2. Least Recently Used (LRU)\n3. Optimal\n4. Exit\n\nYour Choice: ");
        if (scanf("%d", &ch) != 1) break;
        switch (ch) {
            case 1: fifo(pages, n, frameSize); break;
            case 2: lru(pages, n, frameSize); break;
            case 3: optimal(pages, n, frameSize); break;
            case 4: return 0;
            default: printf("\nInvalid choice! Please try again!\n");
        }
    } while (ch != 4);

    return 0;
}

//OutPut
/*

ayush@Legion:~/Ayush_I3104$ gcc Ass_6.c -o pager
ayush@Legion:~/Ayush_I3104$ ./pager
Enter the reference string (chars, spaces allowed): 1 2 3 4 1 2 5 1 2 3 4 5
Enter the size of the frame: 3

MENU
====
1. FIFO
2. Least Recently Used (LRU)
3. Optimal
4. Exit

Your Choice: 1

Data Requested	Frame contents	    Status
==============================================
	1		1   		FAULT
	2		1   2   		FAULT
	3		1   2   3   		FAULT
	4		4   2   3   		FAULT
	1		4   1   3   		FAULT
	2		4   1   2   		FAULT
	5		5   1   2   		FAULT
	1		5   1   2   		HIT
	2		5   1   2   		HIT
	3		5   3   2   		FAULT
	4		5   3   4   		FAULT
	5		5   3   4   		HIT

==============================================

Total no. of Page Faults: 9


MENU
====
1. FIFO
2. Least Recently Used (LRU)
3. Optimal
4. Exit

Your Choice: 2

Data Requested	Frame contents	    Status
==============================================
	1		1   		FAULT
	2		1   2   		FAULT
	3		1   2   3   		FAULT
	4		4   2   3   		FAULT
	1		4   1   3   		FAULT
	2		4   1   2   		FAULT
	5		5   1   2   		FAULT
	1		5   1   2   		HIT
	2		5   1   2   		HIT
	3		3   1   2   		FAULT
	4		3   4   2   		FAULT
	5		3   4   5   		FAULT

==============================================

Total no. of Page Faults: 10


MENU
====
1. FIFO
2. Least Recently Used (LRU)
3. Optimal
4. Exit

Your Choice: 3

Data Requested	Frame contents	    Status
==============================================
	1		1   		FAULT
	2		1   2   		FAULT
	3		1   2   3   		FAULT
	4		1   2   4   		FAULT
	1		1   2   4   		HIT
	2		1   2   4   		HIT
	5		1   2   5   		FAULT
	1		1   2   5   		HIT
	2		1   2   5   		HIT
	3		3   2   5   		FAULT
	4		4   2   5   		FAULT
	5		4   2   5   		HIT

==============================================

Total no. of Page Faults: 7


MENU
====
1. FIFO
2. Least Recently Used (LRU)
3. Optimal
4. Exit

Your Choice: 4
ayush@Legion:~/Ayush_I3104$


*/
