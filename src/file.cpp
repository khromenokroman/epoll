#include "file.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <memory>

#include <iostream>

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
