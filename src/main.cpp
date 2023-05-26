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
        printf("file 1.txt fd: %d\n", fd_text);
        int efd = epoll_create(1);
        if (efd ==-1)
        {
            printf("Error epoll create!!!\n");
            return -1;
        }
        printf("epoll fd: %d\n", efd);
        struct epoll_event ev1;
        ev1.data.fd = fd_text;
        ev1.events = EPOLLIN;
        if (epoll_ctl(efd, EPOLL_CTL_ADD, fd_text, &ev1)) //в книге написанно что нельяз передовать дескриптор файла
        {
            printf("Error epoll ctl!!!\n");
            return -1;
        }

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
                printf("Error epoll wait!!!\n");
                return -1;
            }
            if (!res)
            {
                printf("res: %d\n", res);
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