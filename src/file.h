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
    struct pollfd fds;                      // инициализация структуры poll
    int fd = -1;                            // файловый дескриптор
    ssize_t num_pread;                      // сколько байт причали
    int result;                             // готовность дескрипторов
    off_t off_s;                            // смещение источника
    size_t size_buffer;                     // размер буфера
    std::unique_ptr<char[]> buf;            // буфер в динамической памяти
    bool status = false;                    // признак того что надо ли запускать мониторинг
    void start();                           // запустить мониторинг
    bool stop();                            // остановить мониторинг
    void write_file(size_t bytes_to_write); // записать данные в файл

public:
    const char *file_name; // название файла
    enum class Mode
    {
        read,
        write
    };
    int read_file(void *buf, size_t size_buf, off_t offset); // читаем из файла pread  по смещению
    int write_file(void *buf, size_t size_buf, off_t offset); // пишем в файл pwrite  по смещению
    size_t get_size_file();                                  // узнать размер файла
    size_t get_fd();                                         // узнать дескриптор
    File(const char *file_name, Mode);                       // коструктор который берет и открывает файл на чтение или на запись
    ~File();                                                 // деструктор
};

class Buffer // буфер
{
private:
    size_t size_buffer;          // размер буфера
    std::unique_ptr<char[]> buf; // буфер
public:
    Buffer(size_t size_buffer)
    {
        buf = std::unique_ptr<char[]>(new char[size_buffer]);
        this->size_buffer = size_buffer;
    }
    size_t get_size_buffer(); // получить размер буфера
    void *get_buffer();       // получить сам буфер
};

class My_error final : std::exception // свои исключения
{
public:
    explicit My_error(std::string &&message) noexcept : message{std::move(message)} {}
    const char *what() const noexcept override { return message.c_str(); } // переопределим what
private:
    std::string message; // сообщение
};
