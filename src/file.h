#pragma once

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <memory>
#include <string.h>
#include <iostream>

class File;

class Buffer final // буфер
{
private:
    size_t size_buffer;          // размер буфера
    std::unique_ptr<char[]> buf; // буфер
    // size_t get_size_buffer();    // получить размер буфера
    // void *get_buffer();          // получить сам буфер

public:
    Buffer(size_t size_buffer); // конструктор создания
    friend File;
};

class File final
{
private:
    const char *file_name; // название файла
    int fd = -1;           // файловый дескриптор
    off_t offset = 0;      // смещение курсора

public:
    enum class Mode // операция с файлом
    {
        read,
        write
    };

    size_t read_file(Buffer &buf, size_t bytes_to_read);   // читаем из файла pread  по смещению
    size_t write_file(Buffer &buf, size_t bytes_to_write); // пишем в файл pwrite  по смещению

    File(const char *file_name, Mode); // коструктор который берет и открывает файл на чтение или на запись
    ~File();                           // деструктор
    // friend Buffer;
};

// свои исключения для открытия файла
class Open_error final : std::exception // свои исключения для открытия файла
{
public:
    explicit Open_error(int cod_error) noexcept : message{}
    {
        std::cerr << "Ошибка работы с файлом!!!\n"
                  << "Код ошибки: " << cod_error << " - " << strerror(cod_error) << "\n";
    }

private:
    std::string message;
};

class Poll_error final : std::exception // свои исключения для poll
{
public:
    explicit Poll_error(int cod_error) noexcept : message{}
    {
        std::cerr << "Ошибка работы с POLL!!!\n"
                  << "Код ошибки: " << cod_error << " - " << strerror(cod_error) << "\n";
    }

private:
    std::string message;
};

class Write_error final : std::exception // свои исключения для записи файла
{
public:
    explicit Write_error(int cod_error) noexcept : message{}
    {
        std::cerr << "Ошибка работы с записью в файла!!!\n"
                  << "Код ошибки: " << cod_error << " - " << strerror(cod_error) << "\n";
    }

private:
    std::string message;
};

class Read_error final : std::exception // свои исключения для чтения файла
{
public:
    explicit Read_error(int cod_error) noexcept : message{}
    {
        std::cerr << "Ошибка работы с чтением из файла!!!\n"
                  << "Код ошибки: " << cod_error << " - " << strerror(cod_error) << "\n";
    }

private:
    std::string message;
};
