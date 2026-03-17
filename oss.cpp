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

# user inputs oss[-h][-n][-s][-t][-i][-f]
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

	# setup signal/alarm

	# oss sets up message queue
        #define PERMS 0644
        typedef struct msgbuffer {
                long mtype;
                char strData[100];
                int intData;
        } msgbuffer;

	# initializ process table
	struct PCB {
		int occupied;
		pid_t pid;
		int startSeconds;
		int startNano;
		int endingTimeSeconds;
		int endingTimeNano;
		int messagesSent
	};
	struct PCB processTable[20];

	# oss initializes system clock

	struct SimulatedClock {
        	unsigned int seconds;
        	unsigned int nanoseconds;
	};

	# while there's still children
		#increment clock
		# fork + exec child if able
		# pick next child
		# send/ receive messages
		# if child done, wait(), update PCB


	# output summary (total number of processes launched, number of times messages were sent from oss)

	return 0;
}
