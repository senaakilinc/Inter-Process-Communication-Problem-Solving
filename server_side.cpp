# CopyRight (C) 	: Muhammet Gökhan CİNSDİKİCİ
# Date                     : 5 December 2023
# Code Repo           : IPC-ProblemSolution
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <string.h>

// Define structure for message queue
struct msg_buffer {
    long msg_type;
    char msg_text[100];
} receivedMsg, sendedMsg;

int main() {
    // Array of new program versions
    char* newVersions[6]; // Initialize with actual versions
    // Array of URLs corresponding to new versions
    char* newURLs[6]; // Initialize with actual URLs

    // Creating keys and setting up message queue
    key_t msgKey = ftok("/home", 'M');
    int msgid = msgget(msgKey, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("Failed to create/access message queue");
        exit(EXIT_FAILURE);
    }

    // Creating key for shared memory
    key_t shmKey = ftok("/temp", 'C');
    int shmID;

    // Server's ID for message queue operations
    int serverID = 0;

    // Server loop
    while (1) {
        // Receiving messages from clients
        if (msgrcv(msgid, &receivedMsg, sizeof(receivedMsg.msg_text), serverID, 0) == -1) {
            perror("Error receiving message");
            exit(EXIT_FAILURE);
        } else {
            // Check received message against new versions
            for (int i = 0; i < sizeof(newVersions) / sizeof(newVersions[0]); i++) {
                // Extract program ID from message
                char* programID = strtok(receivedMsg.msg_text, " ");
                char* versionID = strtok(NULL, " ");

                // Check if the program ID matches and requires an update
                if (strcmp(programID, newVersions[i]) == 0) {
                    if (strcmp(receivedMsg.msg_text, newVersions[i]) == 0) {
                        // Send back the same message if up to date
                        strcpy(sendedMsg.msg_text, newVersions[i]);
                        sendedMsg.msg_type = strtol(programID, NULL, 10);
                        msgsnd(msgid, &sendedMsg, sizeof(sendedMsg.msg_text), 0);
                    } else {
                        // Create shared memory and send updated info
                        shmID = shmget(shmKey, 30, IPC_CREAT | 0666);
                        char* shared_data = (char*)shmat(shmID, NULL, 0);
                        strcpy(shared_data, newURLs[i]);

                        sprintf(sendedMsg.msg_text, "%s %d", newVersions[i], shmID);
                        sendedMsg.msg_type = strtol(programID, NULL, 10);
                        msgsnd(msgid, &sendedMsg, sizeof(sendedMsg.msg_text), 0);
                    }
                    break;
                }
            }
        }
    }
    return 0;
}
