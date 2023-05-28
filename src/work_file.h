#pragma once

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <iostream>

class File
{
private:
    struct pollfd fds; // инициализация структуры

    int fd_in, fd_out;                       // файловые дескрипторы
    ssize_t num_pread;                       // сколько байт причали
    int result;                              // готовность дескрипторов
    char const *file_name_in;                // имя файла за которым смотрим
    char const *file_name_out;               // имя файла куда пишем
    static const size_t SIZE_BUFFER = 4096;  // размер буфера
    char buf[SIZE_BUFFER];                   // буффер
    off_t off_s = lseek(fd_in, 0, SEEK_SET); // смещение на начало
public:
    File(char const *file_name_in, char const *file_name_out);
    ~File();
};

class My_error final : std::exception // сделаем свои ошибки
{
public:
    explicit My_error(const std::string &&message) noexcept : message{std::move(message)} {}
    const char *what() const noexcept override { return message.c_str(); } // message
private:
    std::string message; // message
};