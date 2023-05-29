#include "file.h"

#include <iostream>

int main()
{
    File input("input.txt", File::Mode::read);

    std::cout << "Размер файла " <<input.file_name << " " << input.get_size_file() << " байт\n";

    Buffer buffer(4096);
    std::cout << "Размер буфера: " << buffer.get_size_buffer() << "\n";
    
    std::cout << input.read_file(buffer.get_buffer(), buffer.get_size_buffer(),0) << std::endl;
}
