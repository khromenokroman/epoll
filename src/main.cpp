#include "work_file.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include<iostream>

int main()
{
    try
    {
        File a("input.txt", "output.txt");
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
