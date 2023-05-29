#include "open_file.h"

Open_file::Open_file(const char *file_name, Type_oerations operations)
{
    if (operations == wr)
    {
        fd = open(file_name, O_RDWR | O_CREAT, 0770); // открывам файл приемник, чтение\запись, создать, добавлять. rwxrwx---
        if (fd == -1)                                 // проверим на ошибку открытия
        {
            std::cerr << "Не могу открыть файл приемник!\n";
        }
    }
    else if (operations == rd)
    {
        fd = open(file_name, O_RDONLY | O_NONBLOCK); // открывам файл источник, только чтение, неблокирующий. rwxrwx---
        if (fd == -1)                                // проверим на ошибку открытия
        {
            std::cerr << "Не могу открыть файл источник!\n";
        }
    }
    else
    {
        std::cerr << "Переданный мне параметр для работы с файлом не известен!!\n";
    }
}

Open_file::~Open_file()
{
    if (fd > 0) //проверим открыт ли дескриптор
    {
        if (close(fd) == -1)
        {
            std::cerr << "[ОШИБКА] Не могу закрыть файловый дескриптор!\n";
        }
    }
}