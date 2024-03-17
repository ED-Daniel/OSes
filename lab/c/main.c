#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_PATTERN_LENGTH 256

int counter = 0;
char filename[MAX_FILENAME_LENGTH];
char pattern[MAX_PATTERN_LENGTH];

void sigtrap(int signum)
{
    counter++;

    printf("WARNING: CTRL+C received!");

    if (counter > 10)
    {
        printf("Execution terminated due to more than 10 interrupt signals received\n");
        exit(1);
    }

    if (filename[0] == '\0')
    {
        printf("\nEnter file name: ");
    }

    if (pattern[0] == '\0' && filename[0] != '\0')
    {
        printf("\nEnter searching pattern (q - quit): ");
    }
}

void check_output_access() {
    if (access("output.txt", W_OK) == -1) {
        perror("output.txt needs writing permissions");
        exit(1);
    }
}

void check_writer_access() {
    if (access("writer.exe", X_OK) == -1 || !fopen("writer.exe", "r")) {
        perror("writer.exe needs executing permissions or not found");
        exit(1);
    }
}

void read_file_name()
{
    printf("Enter file name: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    while (filename[0] == '\0' || access(filename, R_OK) == -1 || !fopen(filename, "r"))
    {
        printf("File does not exist or permission was denied!\n");
        printf("Enter file name: ");
        fgets(filename, sizeof(filename), stdin);
        filename[strcspn(filename, "\n")] = '\0';
    }
}

void read_pattern()
{
    printf("Enter searching pattern (q - quit): ");
    fgets(pattern, sizeof(pattern), stdin);
    pattern[strcspn(pattern, "\n")] = '\0';

    while (pattern[0] == '\0')
    {
        printf("Enter non-empty string!\n");
        printf("Enter searching pattern (q - quit): ");
        fgets(pattern, sizeof(pattern), stdin);
        pattern[strcspn(pattern, "\n")] = '\0';
    }
}

int main()
{
    check_output_access();
    check_writer_access();

    memset(filename, 0, MAX_FILENAME_LENGTH);
    memset(pattern, 0, MAX_PATTERN_LENGTH);

    struct sigaction sigact;
    sigact.sa_handler = sigtrap;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGINT, &sigact, NULL);

    while (1)
    {
        read_file_name();
        read_pattern();

        if (strcmp(pattern, "q") == 0)
        {
            break;
        }

        int fd[2];
        if (pipe(fd) == -1)
        {
            printf("Failed to create pipe\n");
            exit(1);
        }

        pid_t pid = fork();
        if (pid == -1)
        {
            printf("Failed to create child process\n");
            exit(1);
        }

        if (pid == 0)
        {
            // Child process
            close(fd[0]); // Закрыть конец для чтения

            dup2(fd[1], STDOUT_FILENO); // Перенаправить стандартный вывод в pipe

            execl("/usr/bin/grep", "grep", pattern, filename, NULL);

            printf("Failed to execute command\n");
            exit(1);
        }
        else
        {
            // Parent process
            close(fd[1]); // Закрыть конец для записи

            char result[1024];
            ssize_t bytesRead;
            while ((bytesRead = read(fd[0], result, sizeof(result) - 1)) > 0)
            {
                result[bytesRead] = '\0'; // Добавить символ конца строки

                // Process the result
                if (fork() == 0) {
                    execl("./writer.exe", "writer.exe", result, NULL);
                    printf("Failed to execute writer\n");
                    exit(1);
                }
            }

            if (bytesRead == -1)
            {
                printf("Failed to read from pipe\n");
                exit(1);
            }

            close(fd[0]); // Закрыть конец для чтения

            filename[0] = '\0';
            pattern[0] = '\0';
        }
    }

    printf("Program terminated\n");

    return 0;
}
