#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

int main()
{
    struct pollfd fds; // инициализация структуры

    int fd_in, fd_out;         // файловые дескрипторы
    ssize_t num_pread;         // сколько байт причали
    int result;                // готовность дескрипторов
    char const *file_name_in;  // имя файла за которым смотрим
    char const *file_name_out; // имя файла куда пишем
    size_t size_buffer = 10; // размер буфера
    char buf[size_buffer];     // буффер
    off_t off_s;               // смещение

    file_name_in = "input.txt"; // имена файлов
    file_name_out = "output.txt";

    fd_in = open(file_name_in, O_RDONLY | O_NONBLOCK); // открывам файл источник, только чтение, неблокирующий. rwxrwx---
    if (fd_in == -1)                                   // проверим на ошибку открытия
    {
        printf("Не могу открыть файл источник!\n");
        return -1;
    }
    off_s = lseek(fd_in, 0, SEEK_SET); // начало файла источника


    fd_out = open(file_name_out, O_RDWR | O_CREAT | O_APPEND, 0770); // открывам файл приемник, чтение\запись, создать, добавлять. rwxrwx---
    if (fd_in == -1)                                                 // проверим на ошибку открытия
    {
        printf("Не могу открыть файл приемник!\n");
        return -1;
    }

    fds.fd = fd_in;      // добавим файл источник в мониторинг
    fds.events = POLLIN; // события, происходящие с файловым дескриптором

    while (true)
    {
        result = poll(&fds, 1, -1); // узнаем что готово
        if (result == -1)           // проверим на ошибку
        {
            printf("Не могу получить список готовности файлов!\n");
            return -1;
        }
        else if (result > 0)
        {
            if (fds.revents & POLLIN)
            {
                while ((num_pread = pread(fd_in, buf, size_buffer, off_s)) > 0) // читаем пока есть что читать
                {
                    printf("Позиция курсора: %ld\n", off_s);
                    printf("Считанно байтов: %ld\n", num_pread);
                    printf("Буфер: %.*s\n", size_buffer, buf);
                    off_s += num_pread;
                    printf("Позиция курсора: %ld\n", off_s);
                    // for (int i=0;i<size_buffer;i++)
                    // {
                    //     buf[i]=0;
                    // }
                }
            }
        }
        // return 0;
    }

    close(fd_in); // чистим ресурсы
    close(fd_out);
}