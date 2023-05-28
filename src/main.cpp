#include "work_file.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

int main()
{
    // struct pollfd fds; // инициализация структуры

    // int fd_in, fd_out;         // файловые дескрипторы
    // ssize_t num_pread;         // сколько байт причали
    // int result;                // готовность дескрипторов
    // char const *file_name_in;  // имя файла за которым смотрим
    // char const *file_name_out; // имя файла куда пишем
    // size_t size_buffer = 4096;   // размер буфера
    // char buf[size_buffer];     // буффер
    // off_t off_s;               // смещение

    // file_name_in = "input.txt"; // имена файлов
    // file_name_out = "output.txt";

    // File::File()

    // fd_in = open(file_name_in, O_RDONLY | O_NONBLOCK); // открывам файл источник, только чтение, неблокирующий. rwxrwx---
    // if (fd_in == -1)                                   // проверим на ошибку открытия
    // {
    //     printf("Не могу открыть файл источник!\n");
    //     return -1;
    // }
    // // off_s = lseek(fd_in, 0, SEEK_SET); // начало файла источника

    // fd_out = open(file_name_out, O_RDWR | O_CREAT, 0770); // открывам файл приемник, чтение\запись, создать, добавлять. rwxrwx---
    // if (fd_in == -1)                                                 // проверим на ошибку открытия
    // {
    //     printf("Не могу открыть файл приемник!\n");
    //     return -1;
    // }

    fds.fd = fd_in;      // добавим файл источник в мониторинг
    fds.events = POLLIN; // события, происходящие с файловым дескриптором

    while (true)
    {
        result = poll(&fds, 1, -1); // узнаем что готово
        if (result == -1)           // проверим на ошибку
        {
            printf("Не могу получить список готовности файлов!\n");
            return -1;
        }
        else if (result > 0)
        {
            if (fds.revents & POLLIN)
            {
                while ((num_pread = pread(fd_in, buf, size_buffer, off_s)) > 0) // читаем пока есть что читать
                {
                    // printf("Позиция курсора: %ld\n", off_s);
                    // printf("Считанно байтов: %ld\n", num_pread);
                    // printf("Буфер: %.*s\n", size_buffer, buf);
                    
                    if (num_pread < size_buffer) //если размер считанных данных меньше буфера
                    {
                        printf("Начинаем запись в файл данные меньше буфера\n");
                        size_t bytes_to_write = num_pread;
                        for (int bytes_written = 0; bytes_written < bytes_to_write;) // проверим
                        {

                            int currently_written = write(fd_out, buf + bytes_written, bytes_to_write - bytes_written); // запишем в файл
                            if (currently_written == -1)                                                                
                            {
                                printf("Не могу записать в целевой файл!!\n");
                                return -1;
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

                                int currently_written = write(fd_out, buf + bytes_written, bytes_to_write - bytes_written); // запишем в файл
                                if (currently_written == -1)                                                              
                                {
                                    printf("Не могу записать в целевой файл!!\n");
                                    return -1;
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

    close(fd_in); // чистим ресурсы
    close(fd_out);
}