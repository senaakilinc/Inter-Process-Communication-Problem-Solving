# CopyRight (C) 	: Muhammet Gökhan CİNSDİKİCİ
# Date                     : 5 December 2023
# Code Repo           : IPC-ProblemSolution
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>

// Structure for message queue
struct msg_buffer {
    long msg_type;
    char msg_text[100];
} message;

int main() {
    // Program's ID, version, and name
    char programID[] = "0000001 V2.3 CinsCalculator";
    
    // Creating a key for the message queue
    key_t key = ftok("/home", 'M');

    // Attempt to create or access a message queue
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("Message queue creation failed");
        exit(EXIT_FAILURE);
    }

    // Preparing the message to be sent
    message.msg_type = 1;
    strcpy(message.msg_text, programID);

    // Sending the message
    if (msgsnd(msgid, &message, sizeof(message.msg_text), 0) == -1) {
        perror("Failed to send message");
    } else {
        // Receiving the response from the server
        if (msgrcv(msgid, &message, sizeof(message.msg_text), 1, 0) == -1) {
            perror("Failed to receive message");
        } else {
            char *token = strtok(programID, " ");
            if (strcmp(message.msg_text, token) == 0) {
                printf("Program is up to date\n");
            } else {
                // Processing the update message
                int shmid = atoi(strrchr(message.msg_text, ' ') + 1);
                char *shmURL = (char *)shmat(shmid, NULL, 0);
                
                printf("URL is %s\n", shmURL);
                printf("Program is updated at %s\n", asctime(localtime(&(time_t){time(NULL)})));

                // Detaching and removing shared memory
                shmdt(shmURL);
                shmctl(shmid, IPC_RMID, NULL);

                strncpy(programID, message.msg_text, strrchr(message.msg_text, ' ') - message.msg_text);
                printf("New programID: %s\n", programID);
            }
        }
    }

    // Continue with the rest of the program
    // ...

    return 0;
}
