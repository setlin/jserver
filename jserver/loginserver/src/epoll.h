/*=============================================================================
#     FileName: epoll.h
#         Desc: net model,epoll system call
#       Author: Setlin
#      Version: 0.0.1
#       Create: 08:23 2015/03/21
#   LastChange: 08:23 2015/03/21
#      History:
=============================================================================*/
#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <stdbool.h>

// count of epoll process fd
#define EPOLL_PROCESS_COUNT 1024

typedef int event_fd;

// event
struct event
{
    void* data;
    bool iswrite;
    bool isread;
};

// set fd to be nonblocking
int poll_setnonblocking(int fd);

// create epollfd
int poll_create();

// release epollfd
void poll_release(int efd);

// add fd to epollfd
int poll_add(int epollfd, int fd, void* ud);

// delete fd from epollfd
void poll_del(int epollfd, int fd);

// close fd
void poll_close(int fd);

// wait for event
int poll_wait(int epollfd, struct event* evs, int maxevents, int timeout);

// for writing data
void poll_write(int epollfd, int fd, void* ud, bool enable);

// judge the fd is right or not
bool poll_invalid(int efd);

#endif















