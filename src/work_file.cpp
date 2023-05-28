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
        while (true)
        {
            result = poll(&fds, 1, -1); // узнаем что готово
            if (result == -1)           // проверим на ошибку
            {
                My_error("Не могу получить список готовности файлов!");
            }
            else if (result > 0)
            {
                if (fds.revents & POLLIN)
                {
                    while ((num_pread = pread(fd_in, buf.get(), size_buffer, off_s)) > 0) // читаем пока есть что читать
                    {
                        if (num_pread < size_buffer) //если размер считанных данных меньше буфера
                        {
                            printf("Начинаем запись в файл данные меньше буфера\n");
                            size_t bytes_to_write = num_pread;
                            for (int bytes_written = 0; bytes_written < bytes_to_write;) // проверим
                            {

                                int currently_written = write(fd_out, buf.get() + bytes_written, bytes_to_write - bytes_written); // запишем в файл
                                if (currently_written == -1)                                                                
                                {
                                    My_error("Не могу записать в целевой файл!!");
                                }

                                bytes_written += currently_written;
                            }
                            printf("Запись завершена данные меньше буфера\n");
                        }
                        else //если больше
                        {
                            printf("Начинаем запись в файл данные больше буфера\n");
                            for (; num_pread != 0; num_pread - size_buffer)
                            {
                                size_t bytes_to_write = size_buffer;
                                for (int bytes_written = 0; bytes_written < bytes_to_write;) // проверим
                                {

                                    int currently_written = write(fd_out, buf.get() + bytes_written, bytes_to_write - bytes_written); // запишем в файл
                                    if (currently_written == -1)                                                              
                                    {
                                        My_error("Не могу записать в целевой файл!!");
                                    }

                                    bytes_written += currently_written;
                                }
                                num_pread -= size_buffer;
                            }
                            printf("Запись завершена данные больше буфера\n");
                        }
                        off_s += num_pread; //проитерировал смещение
                        printf("Позиция курсора: %ld\n", off_s);
                    }
                }
            }
            // return 0;
        }
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
