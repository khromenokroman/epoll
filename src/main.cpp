#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/epoll.h>
int main()
{
    printf("Start program\n");

    /*parameters*/
    const int8_t MAX_EVENT = 10; // max event
    int data_read_file;          // byte read
    char buf[100];               // buf
    char const *file_name = "p";

    int fd_text = open(file_name, O_RDONLY); // open file
    if (fd_text == -1)                       // maybe error
    {
        printf("Error open file!!\nNeed file 1.txt\n");
        return -1;
    }
    else
    {
        printf("file 1.txt fd: %d\n", fd_text); // show fd
        int efd = epoll_create(2);              // create epoll
        if (efd == -1)                          // maybe error
        {
            printf("Error epoll create!!!\n");
            return -1;
        }
        printf("epoll fd: %d\n", efd); // show fd epoll

        struct epoll_event ev; //create struct monitoring
        ev.data.fd = fd_text;
        ev.events = EPOLLIN;

        
        if (epoll_ctl(efd, EPOLL_CTL_ADD, fd_text, &ev) == -1) //add fd file in epoll
        {
            printf("Error epoll ctl!!!\n");
            return -1;
        }

        while (1)
        {
            struct epoll_event events[MAX_EVENT]; //max events
            int res = epoll_wait(efd, events, MAX_EVENT, -1);
            if (res < 0) //maybe error
            {
                printf("Error epoll wait!!!\n");
                return -1;
            }
            if (!res) //wait
            {
                printf("res: %d\n", res);
                printf("timeout%d\n", rand() % 100);
            }
            else // work data
            {
                for (int i = 0; i < res; i++) //begin in result
                {
                    if (events[i].events & EPOLLIN) //work event
                    {
                        data_read_file = read(events[i].data.fd, buf, 100); //read data in event
                        if (data_read_file == -1) //maybe error
                        {
                            printf("Error read!!!");
                            return -1;
                        }
                        printf("read %d bytes: %.*s", data_read_file, data_read_file, buf); //show data
                    }
                }
            }
        }
    }
    if (close(fd_text) == -1) //claen
    {
        printf("[ERROR] Cannot close file!]");
        return -1;
    }
}