#include "file.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <memory>

File::File(char const *file_name, Mode mode)
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
        throw Open_error(errno); // если передали то что не знаем исключение
        break;
    }
    fd = open(file_name, flags, 0770); // открываем файл с режимом который указали выше
    if (fd == -1)                      // если ошибка открытия, то кидаем исключение
    {
        throw Open_error(errno);
    }
    this->file_name = file_name;
}

size_t File::read_file(Buffer &buf, size_t bytes_to_read) // читаем файл pread
{
    struct pollfd fds; // создадим poll

    fds.fd = fd;         // добавим файл источник в мониторинг
    fds.events = POLLIN; // события, происходящие с файловым дескриптором

    if (bytes_to_read > buf.get_size_buffer()) // проверка, вдруг больше буфера, арбуз не надо :)
    {
        throw Read_error(errno); // создадим исключение
    }

    while (true) // запускаем poll и мониторим
    {
        int result = poll(&fds, 1, 100); // узнаем что готово
        if (result == -1)                // проверим на ошибку
        {
            throw Poll_error(errno); // создадим исключение
        }
        else if (result > 0) // если что то нашел
        {
            if (fds.revents & POLLIN) // проверим событие
            {
                size_t bytes_read = 0;             // начальное значение
                while (bytes_read < bytes_to_read) // читаем пока не прочитаем
                {
                    int currently_read = pread(fd, buf.get_buffer() + bytes_read, bytes_to_read - bytes_read, offset);
                    if (currently_read == -1) // вдруг ошибка
                    {
                        throw Read_error(errno); // создадим исключение
                    }
                    if (currently_read == 0) // если конец файла то все...
                    {
                        break;
                    }
                    bytes_read += currently_read; // добавляем байт
                    offset += currently_read;     // итерируем смещение
                }
                return bytes_read;
            }
        }
    }
}

size_t File::write_file(Buffer &buf, size_t bytes_to_write) // пишем в файл pwrite
{
    struct pollfd fds; // создадим poll

    fds.fd = fd;          // добавим файл источник в мониторинг
    fds.events = POLLOUT; // события, происходящие с файловым дескриптором

    if (bytes_to_write > buf.get_size_buffer()) // проверка, вдруг больше буфера, арбуз не надо :)
    {
        throw Write_error(errno); // создадим исключение
    }
    while (true) // запускаем poll и мониторим
    {
        int result = poll(&fds, 1, 100); // узнаем что готово
        if (result == -1)                // проверим на ошибку
        {
            throw Poll_error(errno); // создадим исключение
        }
        else if (result > 0) // если что то нашел
        {
            if (fds.revents & POLLOUT) // проверим событие
            {
                size_t bytes_written = 0;              // начальное значение
                while (bytes_written < bytes_to_write) // пишем пока не кончатся
                {
                    int currently_written = pwrite(fd, buf.get_buffer() + bytes_written, bytes_to_write - bytes_written, offset);
                    if (currently_written == -1) // вдруг ошибка
                    {
                        throw Write_error(errno); // создадим исключение
                    }

                    bytes_written += currently_written; // добавляем байт
                    offset += currently_written;        // итерируем смещение
                }
                return bytes_written;
            }
        }
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
