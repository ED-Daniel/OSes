#include <stdio.h>

// O_WRONLY, open
#include <fcntl.h>

// close
#include <unistd.h>

// exit
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Пожалуйста, укажите строку в качестве аргумента\n");
        return 1;
    }

    /* 
    Флаг `O_WRONLY` указывает, что файл будет открыт только для записи.
    Флаг `O_CREAT` указывает, что файл будет создан, если он не существует.
    Аргумент `0644` устанавливает права доступа к файлу.
    */

    int fd = open("output.txt", O_WRONLY | O_CREAT, 0644);

    if (fd == -1)
    {
        printf("Error in opening file");
        exit(1);
    }

    lseek(fd, 0, SEEK_END);

    int size = 0;
    while (argv[1][size] != '\0') {
        size++;
    }

    write(fd, argv[1], size);
    write(fd, "\n", 1);

    close(fd);

    return 0;
}