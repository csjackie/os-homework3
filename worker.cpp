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

int main() {
	key_t key;
	int msqid;

	// get same key as oss
	key = ftok(".", 'B');
	if (key == -1) {
		perror("Worker ftok");
		exit(1);
	}
	
	msqid = msgget(key, 0644);
	if (msqid == -1) {
		perror("worker msgget");
		exit(1);
	}

	msgbuffer msg;

	// receive message from oss
	if (msgrcv(msqid, &msg, sizeof(msg), 0, 0) == -1) {
		perror("worker msgrcv");
		exit(1);
	}

	std::cout << "Worker %d received: %s\n", getpid(), msg.strData;

	// modify message and send back
	msg.mtype = 1;
	strcpy(msg.strData, "Reply from worker");

	if (msgsnd(msqid, &msg, sizeof(msg), 0) == -1) {
		perror("worker msgsnd");
		exit(1);
	}

	return 0;
}
