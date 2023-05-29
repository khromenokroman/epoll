#include "file.h"

#include <iostream>

int main()
{
    struct pollfd fds; // создадим мониторинг
    int result;        // результаты мониторинга
    size_t data_read;  // причитанно данных

    File input("input.txt", File::Mode::read);    // откроем на чтение
    File output("output.txt", File::Mode::write); // откроем на запись

    if (input.get_fd() == -1 | output.get_fd() == -1)
    {
        return -1;
    }

    Buffer buffer(4096);

    fds.fd = input.get_fd(); // добавим файл источник в мониторинг
    fds.events = POLLIN;     // будем мониторить доступность чтения

    while (true) // проверка на продолжение
    {
        result = poll(&fds, 1, -1); // узнаем что готово
        if (result == -1)           // проверим на ошибку
        {
            std::cerr << "Не могу получить список готовности файлов!\n";
        }
        else if (result > 0)
        {
            if (fds.revents & POLLIN)
            {
                while ((data_read = input.read_file(buffer.get_buffer(), buffer.get_size_buffer(), input.get_offset())) > 0) // читаем пока есть что читать
                {
                    if (data_read < buffer.get_size_buffer()) // если размер считанных данных меньше буфера
                    {
                        output.write_file(data_read, buffer.get_buffer(), buffer.get_size_buffer(), output.get_offset());
                        input.set_offset(data_read);  // проитерировал смещение
                        output.set_offset(data_read); // проитерировал смещение
                    }
                    else // если больше
                    {
                        for (; data_read != 0; data_read - buffer.get_size_buffer())
                        {
                            output.write_file(data_read, buffer.get_buffer(), buffer.get_size_buffer(), output.get_offset());
                            input.set_offset(data_read);  // проитерировал смещение
                            output.set_offset(data_read); // проитерировал смещение
                            data_read -= buffer.get_size_buffer();
                        }
                    }
                }
            }
        }
    }
}
