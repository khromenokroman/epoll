#pragma once

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <memory>

#include <iostream>

class File final
{
private:
    struct pollfd fds;                     // инициализация структуры poll
    int fd_in, fd_out;                     // файловые дескрипторы
    ssize_t num_pread;                     // сколько байт причали
    int result;                            // готовность дескрипторов
    off_t off_s;                           // смещение источника
    size_t size_buffer = 4096;             // размер буфера
    explicit File(const char *file_input); // конструктор основной
    std::unique_ptr<char[]> buf;           // буфер в динамической памяти
    bool status = false;                   // признак того что надо ли запускать мониторинг
public:
    void start();
    File(const char *file_input, const char *file_output); // конструктор второй
    ~File();
};

class My_error final : std::exception
{
public:
    explicit My_error(const std::string &&message) noexcept : message{std::move(message)} {}
    const char *what() const noexcept override { return message.c_str(); } // message
private:
    std::string message; // message
};
