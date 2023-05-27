#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/epoll.h>
int main()
{
    int s;
    char buf[100];
    int fd_text = open("p", O_RDONLY);
    if (fd_text == -1)
    {
        printf("Error open file!!\nNeed file 1.txt\n");
        return -1;
    }
    else
    {
        printf("file 1.txt fd: %d\n", fd_text);
        int efd = epoll_create(2); //create
        if (efd == -1)
        {
            printf("Error epoll create!!!\n");
            return -1;
        }
        printf("epoll fd: %d\n", efd);

        // fcntl(fd_text, fcntl(fd_text, F_GETFL) | O_NONBLOCK);

        struct epoll_event ev;
        ev.data.fd = fd_text;
        ev.events = EPOLLIN;

        //add epoll fd
        if (epoll_ctl(efd, EPOLL_CTL_ADD, fd_text, &ev) == -1)
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
            int res = epoll_wait(efd, events, MAX_EVENT, -1);
            if (res < 0)
            {
                printf("Error epoll wait!!!\n");
                return -1;
            }
            if (!res)
            {
                // printf("res: %d\n", res);
                // printf("timeout%d\n", rand() % 100);
            }
            else
            {
                for (int i = 0; i < res; i++)
                {
                    if (events[i].events & EPOLLIN)
                    {
                        s = read(events[i].data.fd, buf, 100);
                        if (s == -1)
                        {
                            printf("Error read!!!");
                            return -1;
                        }
                        printf("read %d bytes: %.*s", s, s, buf);
                    }
                }
            }
        }
    }
}