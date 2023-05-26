#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/epoll.h>
int main()
{

    int fd_text = open("1.txt", O_RDONLY | O_NONBLOCK, 0770);
    if (fd_text == -1)
    {
        printf("Error open file!!\nNeed file 1.txt\n");
    }
    else
    {
        printf("fd: %d\n",fd_text);
        int efd = epoll_create1(0);
        struct epoll_event ev1;
        ev1.data.fd = fd_text;
        ev1.events = EPOLLIN;
        epoll_ctl(efd, EPOLL_CTL_ADD, fd_text, &ev1);

        while (1)
        {
            enum
            {
                MAX_EVENT = 10
            };
            struct epoll_event events[MAX_EVENT];
            int res = epoll_wait(efd, events, MAX_EVENT, 500);
            if (res < 0)
            {
                printf("Error!!!");
                abort();
            }
            if (!res)
            {
                printf("res: %d\n",res);
                printf("timeout%d\n", rand() % 100);
            }
            else
            {
                for (int i = 0; i < res; i++)
                {
                    struct epoll_event *cur = &events[i];
                    if (cur->data.fd == fd_text)
                    {
                        printf("file is add!!!");
                    }
                }
            }
        }
    }
}