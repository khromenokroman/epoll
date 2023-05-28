#include "work_file.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

int main()
{
    try
    {
        File a("input.txt","output.txt");
    }
    catch (My_error &ex)
    {
        printf("[ОШИБКА]: %s", ex.what());
    }
    catch (std::exception &ex)
    {
        printf("[ОШИБКА]: %s", ex.what());
    }
}
