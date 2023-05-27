#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

int main()
{
    struct pollfd fds[2]; // инициализация структуры

    int fd_in, fd_out;         // файловые дескрипторы
    int result;                // готовность дескрипторов
    char const *file_name_in;  // имя файла за которым смотрим
    char const *file_name_out; // имя файла куда пишем

    file_name_in = "input.txt"; // имена файлов
    file_name_out = "output.txt";

    fd_in = open(file_name_in, O_RDONLY | O_NONBLOCK, 0770); // открывам файл источник, только чтение, неблокирующий. rwxrwx---
    if (fd_in == -1)                                         // проверим на ошибку открытия
    {
        printf("Не могу открыть файл источник!\n");
        return -1;
    }
    fd_in = open(file_name_out, O_RDWR | O_CREAT | O_APPEND, 0770); // открывам файл приемник, чтение\запись, создать, добавлять. rwxrwx---
    if (fd_in == -1)                                                // проверим на ошибку открытия
    {
        printf("Не могу открыть файл приемник!\n");
        return -1;
    }

    fds[0].fd = fd_in;      // добавим файл источник в мониторинг
    fds[0].events = POLLIN; // событие ввод

    result = poll(fds, 1, 0); // узнаем что готово
    if (result == -1)         // проверим на ошибку открытия
    {
        printf("Не могу получить список готовности фалов!\n");
        return -1;
    }
    printf("Результат: %d\n", result);

    close(fd_in);
    close(fd_out);
}