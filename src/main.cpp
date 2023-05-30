#include "file.h"

#include <iostream>

int main()
{
    try
    {
        File input("input.txt", File::Mode::read);    // откроем на чтение
        File output("output.txt", File::Mode::write); // откроем на запись

        Buffer buffer(10);

        size_t data_read = input.read_file(buffer,20); // читаем данные
        std::cout << data_read << std::endl;
    }
    catch(const Open_error &ex)
    {
        std::cerr << ex.what() << '\n';
    }
    catch(const Write_error &ex)
    {
        std::cerr << ex.what() << '\n';
    }
    catch(const Read_error &ex)
    {
        std::cerr << ex.what() << '\n';
    }
    catch(const std::exception &ex)
    {
        std::cerr << ex.what() << '\n';
    }
   

    // 
    // output.write_file(data_read, buffer);    // пишем данные
}
