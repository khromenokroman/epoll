#include "work_file.h"
#include "open_file.h"
#include "create_buffer.h"

#include <iostream>

int main()
{

    struct pollfd fds;
    int result;
    int size_buffer = 4096;
    std::unique_ptr<char[]> buf;
    buf = std::unique_ptr<char[]>(new char[size_buffer]);

    Open_file input("input.txt", Type_oerations::rd);   // создаем объет который открвыет файл на чтение
    Open_file output("output.txt", Type_oerations::wr); // создаем объет который открвыет файл на запись
    fds.fd = input.get_fd();                            // добавим файл источник в мониторинг
    fds.events = POLLIN;                                // события, происходящие с файловым дескриптором
    while (1)                                           // проверка на продолжение
    {
        result = poll(&fds, 1, -1); // узнаем что готово
        if (result>0)
        {
            if (fds.revents & POLLIN)
            {
                // ....   что то делаем
            }
        }
    }
}
