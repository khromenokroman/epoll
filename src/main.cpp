#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/epoll.h>
int main()
{

    int fd_text = open("1.txt", O_RDONLY, 0770);
    if (fd_text == -1)
    {
        printf("Error open file!!\nNeed file 1.txt\n");
        return -1;
    }
    else
    {
        printf("file 1.txt fd: %d\n", fd_text);
        int efd = epoll_create(5); //create
        if (efd == -1)
        {
            printf("Error epoll create!!!\n");
            return -1;
        }
        printf("epoll fd: %d\n", efd);

        fcntl(fd_text, fcntl(fd_text, F_GETFL) | O_NONBLOCK);

        struct epoll_event ev1;
        ev1.data.fd = fd_text;
        ev1.events = EPOLLIN;
        
        //add epoll fd
        if (epoll_ctl(efd, EPOLL_CTL_ADD, fd_text, &ev1) == -1) // в книге написанно что нельяз передовать дескриптор файла
        {
            printf("Error epoll ctl!!!\n");
            return -1;
        }

        // struct epoll_event ev2;
        // ev2.data.fd = 0;
        // ev2.events = EPOLLIN;
        // if (epoll_ctl(efd, EPOLL_CTL_ADD, 0, &ev2)) //в книге написанно что нельяз передовать дескриптор файла
        // {
        //     printf("Error epoll ctl!!!\n");
        //     return -1;
        // }

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
                // printf("res: %d\n", res);
                // printf("timeout%d\n", rand() % 100);
            }
            else
            {
                for (int i = 0; i < res; i++)
                {
                    struct epoll_event *cur = &events[i];
                    if (cur->data.fd == fd_text)
                    {
                        printf("find\n");
                        return 0;
                    }
                }
            }
        }
    }
}