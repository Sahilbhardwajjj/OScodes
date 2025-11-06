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




























⁠ #include <stdio.h> ⁠ → Standard I/O functions
⁠ #include <unistd.h> ⁠ → UNIX system calls (read, write, close, etc.)
⁠ #include <sys/stat.h> ⁠ → File/fifo info & permissions
⁠ #include <fcntl.h> ⁠ → File control (open, flags)
⁠ #define MAX_BUF 1024 ⁠ → Defines buffer size
⁠ mkfifo() ⁠ → Creates named pipe (FIFO file)
⁠ open() ⁠ → Opens file or FIFO
⁠ read() ⁠ → Reads data from file/FIFO
⁠ write() ⁠ → Writes data to file/FIFO
⁠ close() ⁠ → Closes file/FIFO descriptor
⁠ getchar() ⁠ → Reads a single character from input
⁠ FILE *fp ⁠ → File pointer (for normal files)
⁠ fprintf() ⁠ → Writes formatted data to file
⁠ dprintf() ⁠ → Writes formatted data to a file descriptor
⁠ O_RDONLY ⁠ → Open file for reading only
⁠ O_WRONLY ⁠ → Open file for writing only
⁠ 0777 ⁠ → File permission (read/write/execute for all users)

---

### Code Explanation (In Short)

#### *First Program (Sender Process):*

•⁠  ⁠Creates *two FIFOs*: ⁠ myfifo1 ⁠ and ⁠ myfifo2 ⁠
•⁠  ⁠Takes a string input from user until ⁠ # ⁠
•⁠  ⁠Stores it in buffer ⁠ buff ⁠
•⁠  ⁠Opens ⁠ myfifo1 ⁠ in *write mode* and sends the string
•⁠  ⁠Opens ⁠ myfifo2 ⁠ in *read mode* to receive processed data (from 2nd program)
•⁠  ⁠Displays the final result received

👉 *Purpose:* Sends user input to another process and receives result back.

---

#### *Second Program (Receiver Process):*

•⁠  ⁠Creates the same two FIFOs (⁠ myfifo1 ⁠, ⁠ myfifo2 ⁠)
•⁠  ⁠Opens ⁠ myfifo1 ⁠ in *read mode* and reads the message sent by first program
•⁠  ⁠Counts *words, lines, and characters* in the received message
•⁠  ⁠Stores result in a file ⁠ a.txt ⁠
•⁠  ⁠Sends result back to first process through ⁠ myfifo2 ⁠ (write mode)

👉 *Purpose:* Receives data, processes it (counts lines, words, chars), and sends results back.

---

### *Short Viva Summary*

These two programs demonstrate *Inter-Process Communication (IPC)* using *Named Pipes (FIFOs)*.

•⁠  ⁠The *first program* sends a message to the *second* through ⁠ myfifo1 ⁠.
•⁠  ⁠The *second program* reads it, counts words/lines/chars, and sends the result back through ⁠ myfifo2 ⁠.
  This shows how two processes can *exchange data* using FIFOs in UNIX/Linux.
