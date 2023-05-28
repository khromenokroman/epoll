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
    if (fd_in > 0)                                   // проверим на ошибку открытия
    {
        off_s = lseek(fd_in, 0, SEEK_SET); // ставим смещение в начало документа
    }
    printf("Файл исходник дескриптор: %d\n", fd_in);
}

File::File(const char *file_input, const char *file_output) : File(file_input)
{
    if (fd_in == -1) // проверим на ошибку открытия
    {
        throw My_error("Не могу открыть файл источник!\n");
    }

    fd_out = open(file_output, O_RDWR | O_CREAT, 0770); // открывам файл приемник, чтение\запись, создать, добавлять. rwxrwx---
    if (fd_in == -1)                                    // проверим на ошибку открытия
    {
        throw My_error("Не могу открыть файл приемник!\n");
    }

    printf("Файл приемник дескриптор:: %d\n", fd_out);

    buf = std::unique_ptr<char[]>(new char[size_buffer]); // выделим память

    fds.fd = fd_in;      // добавим файл источник в мониторинг
    fds.events = POLLIN; // события, происходящие с файловым дескриптором

    start();
}

inline void File::write_file(size_t bytes_to_write)
{
    for (int bytes_written = 0; bytes_written < bytes_to_write;) // проверим
    {

        int currently_written = write(fd_out, buf.get() + bytes_written, bytes_to_write - bytes_written); // запишем в файл
        if (currently_written == -1)
        {
            throw My_error("Не могу записать в целевой файл!!\n");
        }

        bytes_written += currently_written;
    }
}

bool File::stop() //проверка остановки
{
    if (open("stop.txt", O_RDONLY) == -1) //если есть файл то останавливаемся
    {
        return true;
    }
    else
    {
        printf("[СТОП] Поступила команда остановки!\n");
        return false;
    }

}

void File::start()
{
    while (stop()) //проверка на продолжение
    {
        result = poll(&fds, 1, -1); // узнаем что готово
        if (result == -1)           // проверим на ошибку
        {
            My_error("Не могу получить список готовности файлов!\n");
        }
        else if (result > 0)
        {
            if (fds.revents & POLLIN)
            {
                while ((num_pread = pread(fd_in, buf.get(), size_buffer, off_s)) > 0) // читаем пока есть что читать
                {
                    if (num_pread < size_buffer) // если размер считанных данных меньше буфера
                    {
                        printf("Начинаем запись в файл данные меньше буфера\n");
                        write_file(num_pread);
                        printf("Запись завершена данные меньше буфера\n");
                    }
                    else // если больше
                    {
                        printf("Начинаем запись в файл данные больше буфера\n");
                        for (; num_pread != 0; num_pread - size_buffer)
                        {
                            write_file(size_buffer);
                            num_pread -= size_buffer;
                        }
                        printf("Запись завершена данные больше буфера\n");
                    }
                    off_s += num_pread; // проитерировал смещение
                    printf("Позиция курсора: %ld\n", off_s);
                }
            }
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
