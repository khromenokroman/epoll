#include "file.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
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
        std::cerr << "Код ошибки: " << errno << " - " << strerror(errno) << "\n";
        throw Open_error("[ОШИБКА] Не знаю такого режима отрытия файла!\n"); // если передали то что не знаем исключение
        break;
    }
    fd = open(file_name, flags, 0770); // открываем файл с режимом который указали выше
    if (fd == -1)                      // если ошибка открытия, то кидаем исключение
    {
        std::cerr << "Код ошибки: " << errno << " - " << strerror(errno) << "\n";
        throw Open_error("[ОШИБКА] Не могу открыть файл");
    }
}

size_t File::get_size_file() { return lseek(fd, 0, SEEK_END); } // получим размер фала
size_t File::get_fd() { return fd; }                            // получим фаловый дескриптор
std::string File::get_file_name() { return file_name; }         // получим имя файла
off_t File::get_offset() { return offset; }                     // получим смещение курсора
void File::set_offset(off_t offset) { this->offset += offset; } // сместить курсор

size_t File::read_file(Buffer &buf, size_t bytes_to_read) // читаем файл pread
{
    if (bytes_to_read > buf.get_size_buffer()) // проверка, вдруг больше буфера, арбуз не надо :)
    {
        std::cerr << "Код ошибки: " << errno << " - " << strerror(errno) << "\n";
        throw Read_error("[ОШИБКА] Размер данных больше буфера!");
    }
    size_t bytes_read = 0;
    while (bytes_read < bytes_to_read) // читаем пока не прочитаем
    {
        int currently_read = pread(fd, buf.get_buffer() + bytes_read, bytes_to_read - bytes_read, offset);
        if (currently_read == -1) // вдруг ошибка
        {
            std::cerr << "Код ошибки: " << errno << " - " << strerror(errno) << "\n";
            throw Read_error("[ОШИБКА] Не могу проитать данные!\n");
        }
        bytes_read += currently_read;
        offset += currently_read;
    }
    return bytes_read;
}

size_t File::write_file(Buffer &buf, size_t bytes_to_write) // пишем в файл pwrite
{
    size_t bytes_written = 0;
    while (bytes_written < bytes_to_write)
    {
        //
        int currently_written = pwrite(fd, buf.get_buffer() + bytes_written, bytes_to_write - bytes_written, offset);
        if (currently_written == -1)
        {
            std::cerr << "Код ошибки: " << errno << " - " << strerror(errno) << "\n";
            throw Write_error("[ОШИБКА] Не могу записать в файл\n");
        }

        bytes_written += currently_written;
    }
    return bytes_written;
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
