#include "file.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <memory>

#include <iostream>

File::File(const char *file_name, Mode mode)
{
    int flags;
    switch (mode)
    {
    case Mode::read:
        flags = O_RDONLY | O_NONBLOCK; // только чтение, неблокирующий. rwxrwx---
        break;
    case Mode::write:
        flags = O_RDWR | O_CREAT; // чтение\запись, создать, добавлять. rwxrwx---
        break;
    default:
        throw Open_error("[ОШИБКА] Не знаю такого режима отрытия файла!\n"); // если передали то что не знаем исключение
        break;
    }
    fd = open(file_name, flags, 0770); // открываем файл с режимом который указали выше
    if (fd == -1)                // если ошибка открытия, то кидаем исключение
    {
        throw Open_error("[ОШИБКА] Не могу открыть файл");
    }
}

size_t File::get_size_file() { return lseek(fd, 0, SEEK_END); } // получим размер фала
size_t File::get_fd() { return fd; }                            // получим фаловый дескриптор
std::string File::get_file_name() { return file_name; }         // получим имя файла
off_t File::get_offset() { return offset; }                     // получим смещение курсора
void File::set_offset(off_t offset) { this->offset += offset; } // сместить курсор

size_t File::read_file(Buffer &buf) // читаем файл pread
{
    int data_read = pread(fd, buf.get_buffer(), buf.get_size_buffer(), offset);
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

void File::write_file(size_t bytes_to_write, Buffer &buf) // пишем в файл pwrite
{
    int bytes_written = 0;
    while (bytes_written < bytes_to_write)
    {
        //
        int currently_written = write(fd, buf.get_buffer() + bytes_written, bytes_to_write - bytes_written);
        if (currently_written == -1)
        {
            throw Write_error("[ОШИБКА] Не могу записать в файл\n");
        }

        bytes_written += currently_written;
    }
}

Buffer::Buffer(size_t size_buffer)
{
    buf = std::unique_ptr<char[]>(new char[size_buffer]);
    this->size_buffer = size_buffer;
}
size_t Buffer::get_size_buffer() { return size_buffer; } // получим разсер буфера
void *Buffer::get_buffer() { return buf.get(); }         // получим указатель

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
