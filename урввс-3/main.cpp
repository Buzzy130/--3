#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#define PROCESS_ID_SIZE 4
#define MESSAGE_SIZE 40

int main() {
    int fd[2];
    pid_t p1, p2, p3;
    char process_id[PROCESS_ID_SIZE];
    char message[MESSAGE_SIZE];

    // �������� ��������������� ������
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    p1 = fork();

    if (p1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (p1 == 0) {
        // ������� P1
        p3 = fork();

        if (p3 < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (p3 == 0) {
            // ������� P3
            // ���������� ������ � �����
            // ������� ������� � P1
            // �������� �������
            // ���������� ������ � �����
            exit(EXIT_SUCCESS);
        }
        else {
            // ������� P1
            // �������� ������� ���������� ������ � �����
            // �������� � ����� ������������� K0
            // ������������� P3
            // �������� ���������� P3
            close(fd[0]);
            strcpy(process_id, "P3");
            write(fd[1], process_id, PROCESS_ID_SIZE);
            close(fd[1]);
            wait(NULL);
            exit(EXIT_SUCCESS);
        }
    }
    else {
        // ������� Po
        p2 = fork();

        if (p2 < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (p2 == 0) {
            // ������� P2
            // ����� ������ �������������
            // ���������� ������ � �����
            // ������� �������
            close(fd[1]);
            read(fd[0], process_id, PROCESS_ID_SIZE);
            close(fd[0]);
            kill(atoi(process_id), SIGUSR1);
            exit(EXIT_SUCCESS);
        }
        else {
            // ������� Po
            // ��������� ������ �� ������
            close(fd[1]);
            read(fd[0], message, MESSAGE_SIZE);
            printf("Received message: %s\n", message);
            close(fd[0]);
            wait(NULL);
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}