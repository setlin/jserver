#include "epoll.h"
#include "log.h"

#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

// set fd to be nonblocking
int poll_setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);

    return old_option;
}

// create epollfd
int poll_create()
{
    return epoll_create(EPOLL_PROCESS_COUNT);
}

// release epollfd
void poll_release(int efd)
{
    close(efd);
}

// add fd to epollfd
int poll_add(int epollfd, int fd, void* ud)
{
    epoll_event event;
    // event.data.fd = fd;
    event.data.ptr = ud;
    event.events = EPOLLIN;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        return 1;
    }

    // TODO: poll_setnonblocking(fd);

    return 0;
}

// delete fd from epollfd
void poll_del(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
}

// close fd
void poll_close(int fd)
{
    close(fd);
}

// wait for event
int poll_wait(int epollfd, struct event* evs, int maxevents, int timeout)
{
    struct epoll_event events[maxevents];
    int number = 0;
    number = epoll_wait(epollfd, events, maxevents, timeout);
    if((number < 0) && (errno != EINTR))
    {
        log_error("epoll failure");
    }
    else
    {
        log_debug("wait number : %d\n", number);
        for(int i = 0; i < number; ++i)
        {
            evs[i].data = events[i].data.ptr;
            unsigned flag = events[i].events;
            evs[i].iswrite = (flag & EPOLLOUT) != 0;
            evs[i].isread = (flag & EPOLLIN) != 0;
        }
    }

    return number;
}

// for writing data
void poll_write(int epollfd, int fd, void* ud, bool enable)
{
    struct epoll_event event;
    event.data.ptr = ud;
    event.events = EPOLLIN | (enable ? EPOLLOUT : 0);

    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

// judge the fd is right or not
bool poll_invalid(int efd)
{
    return efd == -1;
}




































