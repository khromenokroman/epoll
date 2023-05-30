#pragma once

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <memory>
#include <string>

class Buffer final // буфер
{
private:
    size_t size_buffer;          // размер буфера
    std::unique_ptr<char[]> buf; // буфер
public:
    Buffer(size_t size_buffer); // конструктор создания
    size_t get_size_buffer();   // получить размер буфера
    void *get_buffer();         // получить сам буфер
};

class File final
{
private:
    const char *file_name;         // название файла
    int fd = -1;                   // файловый дескриптор
    off_t offset = 0;              // смещение курсора
    std::unique_ptr<char[]> buf;   // буфер в динамической памяти
    size_t get_size_file();        // узнать размер файла
    size_t get_fd();               // узнать дескриптор
    std::string get_file_name();   // узнать имя файла
    off_t get_offset();            // получить смещение курсора
    void set_offset(off_t offset); // прибавим к текущему положению курсора

public:
    enum class Mode // операция с файлом
    {
        read,
        write
    };

    size_t read_file(Buffer &buf);                       // читаем из файла pread  по смещению
    void write_file(size_t bytes_to_write, Buffer &buf); // пишем в файл pwrite  по смещению

    File(const char *file_name, Mode); // коструктор который берет и открывает файл на чтение или на запись
    ~File();                           // деструктор
};

class Open_error final : std::exception // свои исключения для открытия файла
{
public:
    explicit Open_error(std::string &&message) noexcept : message{std::move(message)} {}
    const char *what() const noexcept override { return message.c_str(); } // переопределим what
private:
    std::string message; // сообщение
};

class Write_error final : std::exception // свои исключения для записи файла
{
public:
    explicit Write_error(std::string &&message) noexcept : message{std::move(message)} {}
    const char *what() const noexcept override { return message.c_str(); } // переопределим what
private:
    std::string message; // сообщение
};

class Read_error final : std::exception // свои исключения для чтения файла
{
public:
    explicit Read_error(std::string &&message) noexcept : message{std::move(message)} {}
    const char *what() const noexcept override { return message.c_str(); } // переопределим what
private:
    std::string message; // сообщение
};
