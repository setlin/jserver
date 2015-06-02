/*=============================================================================
#     FileName: timerheap.h
#         Desc: timer
#       Author: Setlin
#      Version: 0.0.1
#       Create: 16:11 2015/05/20
#   LastChange: 16:11 2015/05/20
#      History:
=============================================================================*/

#ifndef _TIMERHEAP_H_
#define _TIMERHEAP_H_

#include "jsstd.h"

#include <iostream>
using std::exception;

#define BUFFER_SIZE 64

class heap_timer;

/*
typedef struct _CLIENT_DATA{
    struct sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    heap_timer* timer;
}client_data;
*/

class heap_timer
{
public:
    heap_timer(int delay)
    {
        expire = time(NULL) + delay;
    }

public:
    time_t expire;
    void (*cb_func)(client_data*);
    client_data* user_data;
};

class time_heap
{
public:
    time_heap(int cap) throw (std::exception) : capacity(cap), cur_size(0)
    {
        array = new heap_timer* [capacity];
        if(!array)
        {
            throw std::exception();
        }
        for(int i = 0; i < capacity; ++i)
        {
            array[i] = NULL;
        }
    }
    time_heap(int size, int capacity, heap_timer** init_array) throw (std::exception) : cur_size(size), capacity(capacity)
    {
        if(capacity < size)
        {
            throw std::exception();
        }

        array = new heap_timer* [capacity];
        if(!array)
        {
            throw std::exception();
        }

        for(int i = 0; i < capacity; ++i)
        {
            array[i] = NULL;
        }

        if(size != 0)
        {
            for(int i = 0; i < size; ++i)
            {
                array[i] = init_array[i];
            }

            for(int i = (cur_size)/2; i > 0; ++i)
            {
                percolate_down(i);
            }
        }
    }

    ~time_heap()
    {
        for(int i = 0; i < cur_size; ++i)
        {
            delete array[i];
        }

        delete [] array;
    }

public:
    void add_timer(heap_timer* timer) throw (std::exception)
    {
        if(!timer)
        {
            throw std::exception();
        }

        if(cur_size >= capacity)
        {
            resize();
        }

        int hole = cur_size++;
        int parent = 0;
        for(; hole > 0; hole = parent)
        {
            if(array[parent]->expire <= timer->expire)
            {
                break;
            }

            array[hole] = array[parent];
        }
        array[hole] = timer;
    }

    void del_timer(heap_timer* timer)
    {
        if(!timer)
        {
            return;
        }

        timer->cb_func = NULL;
    }

    heap_timer* top() const
    {
        if(empty())
        {
            return NULL;
        }

        return array[0];
    }

    void pop_timer()
    {
        if(empty())
        {
            return;
        }

        if(array[0])
        {
            delete array[0];
            array[0] = array[--cur_size];

            if(array[0])
            {
                percolate_down(0);
            }
        }
    }

    void tick()
    {
        heap_timer* tmp = array[0];
        time_t cur = time(NULL);

        while(!empty())
        {
            if(!tmp)
            {
                break;
            }
            if(tmp->expire > cur)
            {
                break;
            }

            if(array[0]->cb_func)
            {
                array[0]->cb_func(array[0]->user_data);
            }

            pop_timer();
            tmp = array[0];
        }
    }

    bool empty() const
    {
        return cur_size == 0;
    }

private:
    void percolate_down(int hole)
    {
        heap_timer* temp = array[hole];
        int child = 0;

        for(; (hole*2+1) <= cur_size; hole = child)
        {
            child = hole*2+1;
            if((child < (cur_size -1)) &&(array[child+1]->expire < array[child]->expire))
            {
                ++child;
            }

            if(array[child]->expire < temp->expire)
            {
                array[hole] = array[child];
            }
            else
            {
                break;
            }
        }
        array[hole] = temp;
    }

    void resize() throw (std::exception)
    {
        heap_timer** temp = new heap_timer* [2*capacity];
        for(int i = 0; i < 2*capacity; ++i)
        {
            temp[i] = NULL;
        }

        if(!temp)
        {
            throw std::exception();
        }

        capacity = 2*capacity;
        for(int i = 0; i < cur_size; ++i)
        {
            temp[i] = array[i];
        }

        delete [] array;
        array = temp;
    }


private:
    heap_timer** array;
    int capacity;
    int cur_size;

};



#endif

























