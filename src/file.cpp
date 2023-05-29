#include "file.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <memory>

#include <iostream>

// File::File(size_t size_buffer)
// {
//     std::cout << fd_in << "\t" << fd_out << std::endl;
//     buf = std::unique_ptr<char[]>(new char[size_buffer]); // выделим память
//     this->size_buffer = size_buffer;

//     fds.fd = fd_in;      // добавим файл источник в мониторинг
//     fds.events = POLLIN; // события, происходящие с файловым дескриптором
//     start();
// }

File::File(const char *file_name, Mode mode)
{
    if (mode == Mode::read)
    {
        fd = open(file_name, O_RDONLY | O_NONBLOCK); // открывам файл источник, только чтение, неблокирующий. rwxrwx---
        if (fd == -1)                                // проверим на ошибку открытия
        {
            std::cerr << "Не могу открыть " << file_name << "\n";
        }
        else
        {
            this->file_name = file_name;
        }
    }
    else if (mode == Mode::write)
    {
        fd = open(file_name, O_RDWR | O_CREAT, 0770); // открывам файл приемник, чтение\запись, создать, добавлять. rwxrwx---
        if (fd == -1)                                 // проверим на ошибку открытия
        {
            std::cerr << "Не могу открыть " << file_name << "\n";
        }
        else
        {
            this->file_name = file_name;
        }
    }
    else
    {
        std::cerr << "Не знаю такого режима открытия файла!\n";
    }
}

size_t File::get_size_file() { return lseek(fd, 0, SEEK_END); } // получим размер фала
size_t File::get_fd() { return fd; }                            // получим фаловый дескриптор
std::string File::get_file_name() { return file_name; }         // получим имя файла
off_t File::get_offset() { return offset; }                     // получим смещение курсора
void File::set_offset(off_t offset) { this->offset += offset; } // сместить курсор

int File::read_file(void *buf, size_t size_buf, off_t offset) // читаем файл pread
{
    int data_read = pread(fd, buf, size_buf, offset);
    if (data_read == -1)
    {
        std::cerr << "Не могу проитать данные из " << file_name << "\n";
        return -1;
    }
    else
    {
        return data_read;
    }
}

void File::write_file(size_t bytes_to_write, void *buf, size_t size_buf, off_t offset) // пишем в файл pwrite
{
    for (int bytes_written = 0; bytes_written < bytes_to_write;)
    {

        int currently_written = write(fd, buf + bytes_written, bytes_to_write - bytes_written);
        if (currently_written == -1)
        {
            std::cerr << "Не могу записать в " << file_name << "\n";
        }

        bytes_written += currently_written;
    }
}

size_t Buffer::get_size_buffer() { return size_buffer; } // получим разсер буфера
void *Buffer::get_buffer() { return buf.get(); }

// File::File(const char *file_input, const char *file_output) : File(file_input)
// {
//     if (fd_in == -1) // проверим на ошибку открытия
//     {
//         throw My_error("Не могу открыть файл источник!\n");
//     }

//     buf = std::unique_ptr<char[]>(new char[size_buffer]); // выделим память

//     fds.fd = fd_in;      // добавим файл источник в мониторинг
//     fds.events = POLLIN; // события, происходящие с файловым дескриптором

//     start();
// }

// void File::write_file(size_t bytes_to_write)
// {
//     for (int bytes_written = 0; bytes_written < bytes_to_write;) // проверим
//     {

//         int currently_written = write(fd_out, buf.get() + bytes_written, bytes_to_write - bytes_written); // запишем в файл
//         if (currently_written == -1)
//         {
//             throw My_error("Не могу записать в целевой файл!!\n");
//         }

//         bytes_written += currently_written;
//     }
// }

// bool File::stop() // проверка остановки
// {
//     if (open("stop.txt", O_RDONLY) == -1) // если есть файл то останавливаемся
//     {
//         return true;
//     }
//     else
//     {
//         std::cout << "[СТОП] Поступила команда остановки!\n";
//         return false;
//     }
// }

// void File::start()
// {
//     while (stop()) // проверка на продолжение
//     {
//         result = poll(&fds, 1, -1); // узнаем что готово
//         if (result == -1)           // проверим на ошибку
//         {
//             My_error("Не могу получить список готовности файлов!\n");
//         }
//         else if (result > 0)
//         {
//             if (fds.revents & POLLIN)
//             {
//                 while ((num_pread = pread(fd_in, buf.get(), size_buffer, off_s)) > 0) // читаем пока есть что читать
//                 {
//                     if (num_pread < size_buffer) // если размер считанных данных меньше буфера
//                     {
//                         std::cout << "Начинаем запись в файл данные меньше буфера\n";
//                         write_file(num_pread);
//                         off_s += num_pread; // проитерировал смещение
//                         std::cout << "Запись завершена данные меньше буфера\n";
//                     }
//                     else // если больше
//                     {
//                         std::cout << "Начинаем запись в файл данные больше буфера\n";
//                         for (; num_pread != 0; num_pread - size_buffer)
//                         {
//                             write_file(size_buffer);
//                             off_s += size_buffer; // проитерировал смещение
//                             num_pread -= size_buffer;
//                         }
//                         std::cout << "Запись завершена данные больше буфера\n";
//                     }
//                     std::cout << "Позиция курсора: " << off_s << "\n";
//                 }
//             }
//         }
//     }
// }

File::~File()
{
    if (fd > 0) // проверим а он вообще уществует
    {
        if (close(fd) == -1) // проверим на ошибку
        {
            std::cerr << "Не могу закрыть файл " << file_name << "\n";
        }
    }
}
