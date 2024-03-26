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

    // Создание информационного канала
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
        // Процесс P1
        p3 = fork();

        if (p3 < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (p3 == 0) {
            // Процесс P3
            // Подготовка данных в канал
            // Посылка сигнала в P1
            // Ожидание сигнала
            // Подготовка данных в канал
            exit(EXIT_SUCCESS);
        }
        else {
            // Процесс P1
            // Ожидание сигнала подготовки данных в канал
            // Передача в канал синхронизации K0
            // Идентификация P3
            // Ожидание завершения P3
            close(fd[0]);
            strcpy(process_id, "P3");
            write(fd[1], process_id, PROCESS_ID_SIZE);
            close(fd[1]);
            wait(NULL);
            exit(EXIT_SUCCESS);
        }
    }
    else {
        // Процесс Po
        p2 = fork();

        if (p2 < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (p2 == 0) {
            // Процесс P2
            // Опрос канала синхронизации
            // Подготовка данных в канал
            // Посылка сигнала
            close(fd[1]);
            read(fd[0], process_id, PROCESS_ID_SIZE);
            close(fd[0]);
            kill(atoi(process_id), SIGUSR1);
            exit(EXIT_SUCCESS);
        }
        else {
            // Процесс Po
            // Обработка данных из канала
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