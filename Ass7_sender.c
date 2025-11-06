/*
Name-Ayush Karanjkhele
RollNo.-I3104

ASSIGNMENT 7(A):-
Problem Statement:-
Full duplex communication between two independent processes. 
First process accepts sentences and writes on one pipe to be read 
by second process and second process counts number of 
characters, number of words and number of lines in accepted 
sentences, writes this output in a text file and writes the contents 
of the file on second pipe to be read by first process and displays 
on standard output.

*/
// Ass 7 - Part 1
#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#define Max_Buff 1024

int main()
{
	int fd1, fd2, c = 0;
	char *myfifo1 = "myfifo1";
	char *myfifo2 = "myfifo2";
	char buff[Max_Buff], ch;

	mkfifo(myfifo1, 0777);
	mkfifo(myfifo2, 0777);

	printf("\nEnter the string (end with #):\n");

	while((ch = getchar()) != '#')
		buff[c++] = ch;
	buff[c] = '\0';

	fd1 = open(myfifo1, O_WRONLY);
	write(fd1, buff, c + 1);
	close(fd1);

	fd2 = open(myfifo2, O_RDONLY);
	read(fd2, buff, Max_Buff);
	printf("\nContents of file:\n%s\n", buff);
	close(fd2);

	return 0;
}

// Second code
/*
// Ass 7 - Part 2
#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#define MAX_BUF 1024

int main()
{
	int words = 0, lines = 0, chars = 0, i = 0;
	char buf[MAX_BUF];
	int fd, fd1;
	FILE *fp;

	char *myfifo1 = "myfifo1";
	char *myfifo2 = "myfifo2";

	mkfifo(myfifo1, 0777);
	mkfifo(myfifo2, 0777);

	fd = open(myfifo1, O_RDONLY);
	read(fd, buf, MAX_BUF);
	close(fd);

	printf("\nMessage received:\n%s\n", buf);

	while (buf[i] != '\0')
	{
		if (buf[i] == ' ' || buf[i] == '\n')
			words++;
		else
			chars++;

		if (buf[i] == '\n')
			lines++;

		i++;
	}

	words++;
	lines++;

	fp = fopen("a.txt", "w");
	fprintf(fp, "\nNo. of lines are : %d\n", lines);
	fprintf(fp, "\nNo. of words are : %d\n", words);
	fprintf(fp, "\nNo. of chars are : %d\n", chars);
	fclose(fp);

	fd1 = open(myfifo2, O_WRONLY);
	dprintf(fd1, "\nNo. of lines are : %d\nNo. of words are : %d\nNo. of chars are : %d\n",
	        lines, words, chars);
	close(fd1);

	return 0;
}

*/
//Output
/*

ayush@Legion:~/Ayush_I3104$ gcc Ass7_sen.c -o sender
ayush@Legion:~/Ayush_I3104$ gcc Ass7_sen.c -o sen
ayush@Legion:~/Ayush_I3104$ ./sen

Enter the string (end with #):
Run both processes in separate terminals.
Start any process first — both will wait for each other to communicate.#

Contents of file:

No. of lines are : 2
No. of words are : 19
No. of chars are : 97
— both will wait for each other to communicate.


ayush@Legion:~/Ayush_I3104$ gcc Ass7_receiver.c -o rec
ayush@Legion:~/Ayush_I3104$ ./rec

Message received:
Run both processes in separate terminals.
Start any process first — both will wait for each other to communicate.
ayush@Legion:~/Ayush_I3104$ 

*/
