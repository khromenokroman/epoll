#include "file.h"

#include <iostream>

int main()
{
    File input("input.txt", File::Mode::read);
    File output("output.txt", File::Mode::write);

}
