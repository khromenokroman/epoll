#include "work_file.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

int main()
{
    File file("input.txt", "output.txt");
}