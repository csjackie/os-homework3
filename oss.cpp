#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

// Process Control Block
struct PCB {
	int occupied;
      	pid_t pid;
        int startSeconds;
        int startNano;
        int endingTimeSeconds;
        int endingTimeNano;
        int messagesSent;
};

// Simulated Clock
struct SimulatedClock {
      	unsigned int seconds;
       	unsigned int nanoseconds;
};

//oss sets up message queue
typedef struct msgbuffer {
       	long mtype;
        char strData[100];
        int intData;
} msgbuffer;

// signal handler
void signal_handler(int sig) {
	std::cout << "Timeout reached. Exiting...\n";
	exit(1);
}

//user inputs oss[-h][-n][-s][-t][-i][-f]
int main(int argc, char **argv) {

	// Default values for command line parameters
        int n = 1;
	int s = 1;
	float t = 1;
	float i = 1;
	
	int opt;
	// Currently running children
	int current = 0;
	// Total children ever launched
	int totalLaunched = 0;
	// Set up real time safety timeout (60 seconds)
	signal(SIGALRM, signal_handler);
	alarm(60);

	// Parse command line options
	while ((opt = getopt(argc, argv, "hn:s:t:i:")) != -1) {
		switch (opt) {
			case 'h':
				std::cout << "To run program:\n\t ./oss -n # -s # -t # -i #\n";
				std::cout << "Replace # with an integer for n and s, and a float of t and i.\n";
				return 0;
			// Total number of children to launch
			case 'n': n = atoi(optarg); break;
			// Maximum simultaneous children 
			case 's': s = atoi(optarg); break;
			// Simulated time limit
			case 't': t = atof(optarg); break;
			// Minimum interval between launches
			case 'i': i = atof(optarg); break;
		}
	}

	// prints error message and exits program if the value of n, s, or t are out of range
	if (n <= 0 || n > 100 || s <= 0 || s > 15 || t <= 0 || i < 0 || s > n) {
		std::cout << "Invalid argument values\n";
		exit(1);
	}
	
	// message queue setup
	key_t key;
	int msqid;

	key = ftok(".", 'B');
	if (key == -1) {
		perror("ftok");
		exit(1);
	}

	msqid = msgget(key, 0644 | IPC_CREAT);
	if (msqid == -1) {
		perror("msgget");
		exit(1);
	}

	// Initialize process table
	PCB processTable[20];
	for (int i = 0; i < 20; i++) {
		processTable[i].occupied = 0;
	}

	// Simulated clock
	SimulatedClock clock;
	clock.seconds = 0;
	clock.nanoseconds = 0;

	int index = 0;

	// Launch initial children
	while (totalLaunched < n && current < s) {
		pid_t pid = fork();

		if (pid == 0) {
			execl("./worker", "worker", NULL);
			perror("execl failed");
			exit(1);
		}

		processTable[current].occupied = 1;
		processTable[current].pid = pid;
		processTable[current].messagesSent = 0;

		current++;
		totalLaunched++;
	}
	// main loop
	while (current > 0) {
		// increment clock
		clock.nanoseconds += 100000000;
		if (clock.nanoseconds >= 1000000000) {
			clock.seconds++;
			clock.nanoseconds -= 1000000000;
		}

		// Find next active child
		int found = 0;
		for (int i = 0; i < 20; i++) {
			int idx = (index + i) % 20;
			if (processTable[idx].occupied) {
				index = idx;
				found = 1;
				break;
			}
		}

		if (!found) break;

		pid_t targetPID = processTable[index].pid;


		// send messages
		msgbuffer msg;
		msg.mtype = targetPID;
		msg.intData = 1;

		std::cout << "OSS: Sending message to PID %d at time %u:%u\n", targetPID, clock.seconds, clock.nanoseconds;

		if (msgsnd(msqid, &msg, sizeof(msg), 0) == -1) {
			perror("msgsnd");
		}

		// Receive response
		if (msgrcv(msqid, &msg, sizeof(msg), 1, 0) == -1) {
			perror("msgrcv");
		} else {
			std::cout << "OSS: Received message from PID %d\n", targetPID;
		}

		processTable[index].messagesSent++;

		// Check termination
		if (msg.intData == 0) {
			std::cout << "OSS: Child %d terminating\n", targetPID;

			waitpid(targetPID, NULL, 0);

			processTable[index].occupied = 0;
			current--;

			// Launch new child if needed
			if (totalLaunched < n) {
				pid_t pid = fork();

				if (pid ==0) {
					execl("./worker", "worker", NULL);
					perror("execl failed");
					exit(1);
				}

				processTable[index].occupied = 1;
				processTable[index].pid = pid;
				processTable[index].messagesSent = 0;

				current++;
				totalLaunched++;
			}
		}
	
		index = (index + 1) % 20;
	}

	// clean up
	msgctl(msqid, IPC_RMID, NULL);

	// output summary (total number of processes launched, number of times messages were sent from oss)
	std::cout << "Total children launched: %d\n", totalLaunched;
	
	int totalMessages = 0;

	for (int i = 0; i < 20; i++) {
		totalMessages += processTable[i].messagesSent;
	}

	std::cout << "Total messages sent: %d\n", totalMessages;

	return 0;
}
