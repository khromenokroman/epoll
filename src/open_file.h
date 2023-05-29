#pragma once

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>

enum Type_oerations
{
    wr,
    rd
};

class Open_file
{
private:
    int fd = -1;

public:
    Open_file(const char *file_name, Type_oerations operations); // конструктор который возвращает файловый дескриптор
    int get_fd() { return fd; }                                  // возвращает файловый дескриптор
    ~Open_file();
};