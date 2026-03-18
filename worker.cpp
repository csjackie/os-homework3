#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <iostream>

typedef struct msgbuffer {
	long mtype;
	char strData[100];
	int intData;
} msgbuffer;

int main(int argc, char *argv[]) {
	key_t key;
	int msqid;

	// get message queue
	key = ftok(".", 'B');	
	msqid = msgget(key, 0644);

	msgbuffer msg;
	
	int messagesReceived = 0;
	
	// FAKE termination condition
	int maxLoops = (rand() % 5) + 3;

	std::cout << "Worker PID:%d STARTING\n", getpid();
	
	while (1) {
		// wait for oss
		if (msgrcv(msqid, &msg, sizeof(msg), getpid(), 0) == -1) {
			perror("worker msgrcv");
			exit(1);
		}

		messagesReceived++;

		std::cout << "WORKER PID:%d received message  %d\n", getpid(), messagesReceived;

		// decide if terminating
		if (messagesReceived >= maxLoops) {
			msg.intData = 0;
			std::cout << "WORKER PID:%d TERMINATING\n", getpid();
		} else {
			msg.intData = 1;
		}

		msg.mtype = 1;

		if (msgsnd(msqid, &msg, sizeof(msg), 0) == -1) {
			perror("worker msgsnd");
			exit(1);
		}

		if (msg.intData == 0) break;
	}

	return 0;
}
