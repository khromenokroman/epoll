#include "work_file.h"
#include "open_file.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <iostream>

int main()
{
    try
    {
        Open_file input("input.txt", Type_oerations::rd);
        Open_file output("output.txt", Type_oerations::wr);

        std::cout << "input: " << input.get_fd() << "\n";
        std::cout << "output: " << output.get_fd() << "\n";

        // File create_buf(4096);
        // File a("input.txt", "output.txt");
    }
    catch (My_error &ex)
    {
        std::cerr << "[ОШИБКА]: " << ex.what();
    }
    catch (std::exception &ex)
    {
        std::cerr << "[ОШИБКА]: " << ex.what();
    }
}
