#include "file.h"

#include <iostream>

int main()
{
    File input("input.txt", File::Mode::read);    // откроем на чтение
    File output("output.txt", File::Mode::write); // откроем на запись

    Buffer buffer(10);

    size_t data_read = input.read_file(buffer); // читаем данные
    output.write_file(data_read, buffer);    // пишем данные
}
