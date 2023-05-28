#include "work_file.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <memory>

#include <iostream>

File::File(const char *file_input)
{
    fd_in = open(file_input, O_RDONLY | O_NONBLOCK); // открывам файл источник, только чтение, неблокирующий. rwxrwx---
    if (fd_in == -1)                                 // проверим на ошибку открытия
    {
        My_error("Не могу открыть файл источник!");
    }
    off_s = lseek(fd_in, 0, SEEK_SET); //ставим смещение в начало документа
    printf("Файл исходник дескриптор: %d\n", fd_in);
}

File::File(const char *file_input, const char *file_output) : File(file_input)
{

    fd_out = open(file_output, O_RDWR | O_CREAT, 0770); // открывам файл приемник, чтение\запись, создать, добавлять. rwxrwx---
    if (fd_in == -1)                                    // проверим на ошибку открытия
    {
        My_error("Не могу открыть файл приемник!");
    }

    printf("Файл приемник дескриптор:: %d\n", fd_out);

    buf = std::unique_ptr<char[]>(new char[size_buffer]); // выделим память

    fds.fd = fd_in;      // добавим файл источник в мониторинг
    fds.events = POLLIN; // события, происходящие с файловым дескриптором

    status = true;
}

void File::start()
{
    if (!status)
    {
        printf("Не были проинициализированны файловые дескрипторы или не была выделена память!");
    }
    else
    {
        
    }
}


File::~File()
{
    if (fd_in > 0) // проверим а он вообще уществует
    {
        printf("Закрываю исходный файл!\n");
        if (close(fd_in) == -1) // проверим на ошибку
        {
            printf("Не могу закрыть файл исходник!\n");
        }
    }

    if (fd_out > 0) // проверим а он вообще уществует
    {
        printf("Закрываю файл приемник!\n");
        if (close(fd_out) == -1) // проверим на ошибку
        {
            printf("Не могу закрыть файл приемник!\n");
        }
    }
}
