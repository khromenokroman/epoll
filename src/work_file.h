#pragma once

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <memory>
#include <string>

class File final
{
private:
    enum Type_oerations
    {
        wr,
        rd
    };                                     // операции с файлом
    struct pollfd fds;                     // инициализация структуры poll
    int fd_in = -1, fd_out = -1;           // файловые дескрипторы
    ssize_t num_pread;                     // сколько байт причали
    int result;                            // готовность дескрипторов
    off_t off_s;                           // смещение источника
    static const size_t SIZE_BUFFER = 100; // размер буфера
    std::unique_ptr<char[]> buf;           // буфер в динамической памяти
    bool status = false;                   // признак того что надо ли запускать мониторинг

    File(const char *file_input);           // конструктор основной
    void start();                           // запустить мониторинг
    bool stop();                            // остановить мониторинг
    void write_file(size_t bytes_to_write); // записать данные в файл

public:
    File(const char *file_input, const char *file_output); // конструктор второй
    ~File();                                               // деструктор
};

class My_error final : std::exception // свои исключения
{
public:
    explicit My_error(const std::string &&message) noexcept : message{std::move(message)} {}
    const char *what() const noexcept override { return message.c_str(); } // переопределим what
private:
    std::string message; // сообщение
};
