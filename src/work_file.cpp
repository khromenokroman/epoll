#include "work_file.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <memory>

#include <iostream>

File::File(size_t size_buffer)
{
    buf = std::unique_ptr<char[]>(new char[size_buffer]); // выделим память
    this->size_buffer = size_buffer;

    fds.fd = fd_in;      // добавим файл источник в мониторинг
    fds.events = POLLIN; // события, происходящие с файловым дескриптором

    start();
}

File::File(const char *file_name, Type_oerations operations)
{
    if (operations == wr)
    {
        fd_out = open(file_name, O_RDWR | O_CREAT, 0770); // открывам файл приемник, чтение\запись, создать, добавлять. rwxrwx---
        if (fd_out == -1)                                  // проверим на ошибку открытия
        {
            std::cerr << "Не могу открыть файл приемник!\n";
        }
        else
        {
            std::cout << "Файл приемник дескриптор: " << fd_out << "\n";
        }
    }
    else if (operations == rd)
    {
        fd_in = open(file_name, O_RDONLY | O_NONBLOCK); // открывам файл источник, только чтение, неблокирующий. rwxrwx---
        if (fd_in == -1)                                // проверим на ошибку открытия
        {
            std::cerr << "Не могу открыть файл источник!\n";
        }
        else
        {
            std::cout << "Файл исходник дескриптор: " << fd_in << "\n";
        }
    }
    else
    {
        std::cerr << "Переданный мне параметр для работы с файлом не известен!!\n";
    }
}

// File::File(const char *file_input)
// {
//     fd_in = open(file_input, O_RDONLY | O_NONBLOCK); // открывам файл источник, только чтение, неблокирующий. rwxrwx---
//     std::cout << "Файл исходник дескриптор: " << fd_in << "\n";
// }

// File::File(const char *file_input, const char *file_output) : File(file_input)
// {
//     if (fd_in == -1) // проверим на ошибку открытия
//     {
//         throw My_error("Не могу открыть файл источник!\n");
//     }

//     fd_out = open(file_output, O_RDWR | O_CREAT, 0770); // открывам файл приемник, чтение\запись, создать, добавлять. rwxrwx---
//     if (fd_in == -1)                                    // проверим на ошибку открытия
//     {
//         throw My_error("Не могу открыть файл приемник!\n");
//     }

//     std::cout << "Файл приемник дескриптор: " << fd_out << "\n";

//     buf = std::unique_ptr<char[]>(new char[SIZE_BUFFER]); // выделим память

//     fds.fd = fd_in;      // добавим файл источник в мониторинг
//     fds.events = POLLIN; // события, происходящие с файловым дескриптором

//     start();
// }

void File::write_file(size_t bytes_to_write)
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

bool File::stop() // проверка остановки
{
    if (open("stop.txt", O_RDONLY) == -1) // если есть файл то останавливаемся
    {
        return true;
    }
    else
    {
        std::cout << "[СТОП] Поступила команда остановки!\n";
        return false;
    }
}

void File::start()
{
    while (stop()) // проверка на продолжение
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
                        std::cout << "Начинаем запись в файл данные меньше буфера\n";
                        write_file(num_pread);
                        off_s += num_pread; // проитерировал смещение
                        std::cout << "Запись завершена данные меньше буфера\n";
                    }
                    else // если больше
                    {
                        std::cout << "Начинаем запись в файл данные больше буфера\n";
                        for (; num_pread != 0; num_pread - size_buffer)
                        {
                            write_file(size_buffer);
                            off_s += size_buffer; // проитерировал смещение
                            num_pread -= size_buffer;
                        }
                        std::cout << "Запись завершена данные больше буфера\n";
                    }
                    std::cout << "Позиция курсора: " << off_s << "\n";
                }
            }
        }
    }
}

File::~File()
{
    if (fd_in > 0) // проверим а он вообще уществует
    {
        std::cout << "Закрываю исходный файл!\n";
        if (close(fd_in) == -1) // проверим на ошибку
        {
            std::cout << "Не могу закрыть файл исходник!\n";
        }
    }

    if (fd_out > 0) // проверим а он вообще уществует
    {
        std::cout << "Закрываю файл приемник!\n";
        if (close(fd_out) == -1) // проверим на ошибку
        {
            std::cout << "Не могу закрыть файл приемник!\n";
        }
    }
}
