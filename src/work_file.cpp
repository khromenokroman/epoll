#include "work_file.h"

File::File(char const *file_name_in, char const *file_name_out)
{
    fd_in = open(file_name_in, O_RDONLY | O_NONBLOCK); // открывам файл источник, только чтение, неблокирующий. rwxrwx---
    if (fd_in == -1)                                   // проверим на ошибку открытия
    {
        My_error("Не могу открыть файл источник!");
    }

    fd_out = open(file_name_out, O_RDWR | O_CREAT, 0770); // открывам файл приемник, чтение\запись, создать, добавлять. rwxrwx---
    if (fd_in == -1)                                                 // проверим на ошибку открытия
    {
        My_error("Не могу открыть файл приемник!");
    }
}