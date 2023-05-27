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
    char buf[2];             // буффер
    off_t off_s;               // смещение

    file_name_in = "input.txt"; // имена файлов
    file_name_out = "output.txt";

    fd_in = open(file_name_in, O_RDONLY); // открывам файл источник, только чтение, неблокирующий. rwxrwx---
    if (fd_in == -1)                                   // проверим на ошибку открытия
    {
        printf("Не могу открыть файл источник!\n");
        return -1;
    }

    off_s = lseek(fd_in, 0, SEEK_SET); // начало файла источника

    fd_in = open(file_name_out, O_RDWR | O_CREAT | O_APPEND, 0770); // открывам файл приемник, чтение\запись, создать, добавлять. rwxrwx---
    if (fd_in == -1)                                                // проверим на ошибку открытия
    {
        printf("Не могу открыть файл приемник!\n");
        return -1;
    }

    fds.fd = fd_in;      // добавим файл источник в мониторинг
    fds.events = POLLIN; // события, происходящие с файловым дескриптором

    while (true)
    {
        result = poll(&fds, 1, -1); // узнаем что готово
        if (result == -1)           // проверим на ошибку открытия
        {
            printf("Не могу получить список готовности файлов!\n");
            return -1;
        }
        else if (result > 0)
        {
            if (fds.revents & POLLIN)
            {
                printf("Файл источник готов для чтения\n");
                printf("Позиция курсора: %ld\n", off_s);
                num_pread = pread(fd_in, buf, 2, off_s);
                printf("read bytes: %ld\n", num_pread);
                printf("buffer: %s\n", buf);
                return 0;
            }
        }
    }

    close(fd_in); // чистим ресурсы
    close(fd_out);
}