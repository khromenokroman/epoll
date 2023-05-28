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
    
    fds.fd = fd_in;      // добавим файл источник в мониторинг
    fds.events = POLLIN; // события, происходящие с файловым дескриптором

    while (true)
    {
        result = poll(&fds, 1, -1); // узнаем что готово
        if (result == -1)           // проверим на ошибку
        {
            My_error("Не могу получить список готовности файлов!");
        }
        else if (result > 0)
        {
            if (fds.revents & POLLIN)
            {
                while ((num_pread = pread(fd_in, buf, SIZE_BUFFER, off_s)) > 0) // читаем пока есть что читать
                {
                    if (num_pread < SIZE_BUFFER) //если размер считанных данных меньше буфера
                    {
                        printf("Начинаем запись в файл данные меньше буфера\n");
                        size_t bytes_to_write = num_pread;
                        for (int bytes_written = 0; bytes_written < bytes_to_write;) // проверим
                        {

                            int currently_written = write(fd_out, buf + bytes_written, bytes_to_write - bytes_written); // запишем в файл
                            if (currently_written == -1)                                                                
                            {
                                My_error("Не могу записать в целевой файл!!");
                            }

                            bytes_written += currently_written;
                        }
                        printf("Запись завершена данные меньше буфера\n");
                    }
                    else //если больше
                    {
                        printf("Начинаем запись в файл данные больше буфера\n");
                        for (; num_pread != 0; num_pread - SIZE_BUFFER)
                        {
                            size_t bytes_to_write = SIZE_BUFFER;
                            for (int bytes_written = 0; bytes_written < bytes_to_write;) // проверим
                            {

                                int currently_written = write(fd_out, buf + bytes_written, bytes_to_write - bytes_written); // запишем в файл
                                if (currently_written == -1)                                                              
                                {
                                    My_error("Не могу записать в целевой файл!!\n");
                                }

                                bytes_written += currently_written;
                            }
                            num_pread -= SIZE_BUFFER;
                        }
                        printf("Запись завершена данные больше буфера\n");
                    }
                    off_s += num_pread; //проитерировал смещение
                    printf("Позиция курсора: %ld\n", off_s);
                }
            }
        }
        // return 0;
    }

}