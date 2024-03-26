#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define CHANNEL_KEY 1234
#define SYNC_KEY 5678

struct data {
    int id;
    char message[100];
};

struct msgbuf {
    long mtype;
    struct data mdata;
};

int main() {
    // Create the message queue
    int channel_id = msgget(CHANNEL_KEY, IPC_CREAT | 0666);
    int sync_id = msgget(SYNC_KEY, IPC_CREAT | 0666);

    // Create process P1
    pid_t p1 = fork();

    if (p1 < 0) {
        perror("Fork failed");
        return 1;
    }
    else if (p1 == 0) {
        // P1 process
        // Prepare data
        struct data d1 = { 1, "Data from process P1" };

        // Send data to channel K1
        struct msgbuf buf1 = { 1, d1 };
        msgsnd(channel_id, &buf1, sizeof(struct data), 0);

        // Inform main process about data sent
        printf("Process P1: Data sent to channel K1\n");

        // Send PID of process P3 to process P2 through sync channel K0
        int p3_pid = getpid();
        struct msgbuf buf_sync = { 1, {p3_pid, ""} };
        msgsnd(sync_id, &buf_sync, sizeof(int), 0);

        printf("Process P1: PID of P3 sent to process P2\n");

        // Wait for P3 to finish
        wait(NULL);

    }
    else {
        // Create process P2
        pid_t p2 = fork();

        if (p2 < 0) {
            perror("Fork failed");
            return 1;
        }
        else if (p2 == 0) {
            // P2 process
            // Receive PID of process P3 from P1 through sync channel K0
            struct msgbuf buf_sync;
            msgrcv(sync_id, &buf_sync, sizeof(int), 1, 0);
            int p3_pid = buf_sync.mdata.id;

            printf("Process P2: Received PID of P3 from process P1: %d\n", p3_pid);

            // Send signal to process P3
            kill(p3_pid, SIGUSR1);
        }
        else {
            // Create process P3
            pid_t p3 = fork();

            if (p3 < 0) {
                perror("Fork failed");
                return 1;
            }
            else if (p3 == 0) {
                // P3 process
                // Prepare data
                struct data d3 = { 3, "Data from process P3" };

                // Send data to channel K1
                struct msgbuf buf3 = { 1, d3 };
                msgsnd(channel_id, &buf3, sizeof(struct data), 0);

                // Inform main process about data sent
                printf("Process P3: Data sent to channel K1\n");
            }
            else {
                // Main process
                // Process data from channels
                struct msgbuf buf;
                while (msgrcv(channel_id, &buf, sizeof(struct data), 1, IPC_NOWAIT) != -1) {
                    printf("Main Process: Received data from channel K1 - ID: %d, Message: %s\n", buf.mdata.id, buf.mdata.message);
                }

                // Cleanup message queues
                msgctl(channel_id, IPC_RMID, NULL);
                msgctl(sync_id, IPC_RMID, NULL);

                printf("Main Process: Finished processing\n");
            }
        }
    }

    return 0;
}