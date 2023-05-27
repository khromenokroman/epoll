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
    const int8_t MAX_EVENT = 10;    // max event
    const int8_t BUFFER_SIZE = 100; // buffer size
    int data_read_file;             // byte read
    char buf[BUFFER_SIZE];          // buf
    char const *file_name_read = "p";
    char const *file_name_write = "a";
    int fd_read, fd_write;

    fd_read = open(file_name_read, O_RDONLY); // open file
    if (fd_read == -1)                        // maybe error
    {
        printf("Error open file!!\nNeed file 1.txt\n");
        return -1;
    }
    else
    {
        printf("file 1.txt fd: %d\n", fd_read); // show fd
        int efd = epoll_create(2);              // create epoll
        if (efd == -1)                          // maybe error
        {
            printf("Error epoll create!!!\n");
            return -1;
        }
        printf("epoll fd: %d\n", efd); // show fd epoll

        struct epoll_event ev; // create struct monitoring
        ev.data.fd = fd_read;
        ev.events = EPOLLIN;

        if (epoll_ctl(efd, EPOLL_CTL_ADD, fd_read, &ev) == -1) // add fd file in epoll
        {
            printf("Error epoll ctl!!!\n");
            return -1;
        }

        while (1)
        {
            struct epoll_event events[MAX_EVENT]; // max events
            int res = epoll_wait(efd, events, MAX_EVENT, -1);
            if (res < 0) // maybe error
            {
                printf("Error epoll wait!!!\n");
                return -1;
            }
            if (!res) // wait
            {
                printf("res: %d\n", res);
                printf("timeout%d\n", rand() % 100);
            }
            else // work data
            {
                for (int i = 0; i < res; i++) // begin in result
                {
                    if (events[i].events & EPOLLIN) // work event
                    {
                        data_read_file = read(events[i].data.fd, buf, 100); // read data in event
                        if (data_read_file == -1)                           // maybe error
                        {
                            printf("Error read!!!");
                            return -1;
                        }
                        printf("read %d bytes: %.*s", data_read_file, data_read_file, buf); // show data
                        
                        fd_write = open(file_name_write, O_CREAT | O_APPEND | O_WRONLY, 0770);

                        if (data_read_file != BUFFER_SIZE) // if data < buffer
                        {
                            char buf_new[data_read_file];
                            size_t bytes_to_write = data_read_file;
                            for (int bytes_written = 0; bytes_written < bytes_to_write;) // check
                            {
                                int currently_written = write(fd_write, buf_new + bytes_written, bytes_to_write - bytes_written); // write
                                if (currently_written == -1)                                                                      // maybe error
                                {
                                    printf("[ERROR] Cannot write file!");
                                    return -1;
                                }

                                bytes_written += currently_written; // plus count
                            }
                        }
                        else
                        {
                            for (; data_read_file != 0; data_read_file - BUFFER_SIZE)
                            {
                                size_t bytes_to_write = BUFFER_SIZE;
                                for (int bytes_written = 0; bytes_written < bytes_to_write;) // check
                                {

                                    int currently_written = write(fd_write, buf + bytes_written, bytes_to_write - bytes_written); // write
                                    if (currently_written == -1)                                                                  // maybe error
                                    {
                                        printf("[ERROR] Cannot write file!");
                                        return -1;
                                    }

                                    bytes_written += currently_written; // plus count
                                }
                                data_read_file -= BUFFER_SIZE;
                            }
                        }
                    }
                }
            }
        }
    }
    if (close(fd_read) == -1) // claen
    {
        printf("[ERROR] Cannot close file read!");
        return -1;
    }
    if (close(fd_write) == -1) // claen
    {
        printf("[ERROR] Cannot close file write!");
        return -1;
    }
}