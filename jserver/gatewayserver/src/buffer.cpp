#include "buffer.h"
#include "jsstd.h"
#include "log.h"

// need move ahead or not
// if one buffer is huge, moving it ahead  will waste time, so wast space
#define BUFFERCHAIN_ISMOVE 2
// min buffersize
#define BUFFERCHAIN_MINLEN 6

// node of chain
struct bufferchain
{
    struct bufferchain* next; // the next node
    uint8_t* buffer; // buffer data
    int size; // buffer size
    int pos; // buffer start position
    int length; // data length(used size)
    // remainsize = size - length
};

/*
 * begin           pos      pos +length  size
 * |               |        |            |
 * 0               50       70           100
 *
 * data we need is [pos - pos+length]
 */

// new chain node
static struct bufferchain* bufferchain_new(int buffersize)
{
    // if new node need less size
    // ensure the min length of buffer
    // save memory chip
    if(buffersize < BUFFERCHAIN_MINLEN)
    {
        buffersize = BUFFERCHAIN_MINLEN;
    }

    // count of 'malloc' is equal to count of 'free'
    struct bufferchain* bc = (struct bufferchain*)malloc(sizeof(struct bufferchain));
    bc->next = NULL;
    // malloc buffer data for node
    bc->buffer = (uint8_t*)malloc(buffersize);
    bc->size = buffersize;
    bc->pos = 0;
    bc->length = 0;

    return bc;
}

// delete chain node
static void bufferchain_delete(struct bufferchain** pbc)
{
    // free buffer data
    free((*pbc)->buffer);// TODO: must free string array in struct
    free((*pbc));
    (*pbc) = NULL;
}

// when new data comes
// should move buffer data ahead or not
static int bufferchain_ismoveahead(struct bufferchain* bc, int newdatasize)
{
    return (bc->size - bc->length > newdatasize) &&
        (bc->length <= bc->size/2) &&
        (bc->length <= BUFFERCHAIN_ISMOVE);
}

// move data ahead
static void bufferchain_moveahead(struct bufferchain* bc)
{
    /*sample:
     * buffer = "AAAHelloWorldBBB";
     * size = 128; pos = 2; length = 12;
     * buffer after length or before pos must be useless data
     * "AAA" is a string of used data, we don't it anymore
     * move data ahead
     * "HelloWorldrldBBB"
     * we just need "HelloWorld", string "rldBBB" is useless
     */

    if(bc->pos > 0)
    {
        memmove(bc->buffer, bc->buffer + bc->pos, bc->length);
        bc->pos = 0;
    }
}


// write buffer chain
struct wbuffer
{
    struct bufferchain* head;
    struct bufferchain* tail;
    int length;
};

struct wbuffer* wbuffer_new()
{
    struct wbuffer* wb = (struct wbuffer*)malloc(sizeof(struct wbuffer));
    wb->head = wb->tail = NULL;
    wb->length = 0;
    return wb;
}

void wbuffer_delete(struct wbuffer** pwb)
{
   struct bufferchain* bc = (*pwb)->head;
   while(bc)
   {
        struct bufferchain* tmp = bc->next;
        bufferchain_delete(&bc);
        bc = tmp;
   }

   *pwb = NULL;
}

// pushback new data to wbuffer chain
int wbuffer_pushback(struct wbuffer* wb, const void* data, int datasize)
{
    // if the wbuffer chain is empty, then create
    struct bufferchain* bc = wb->tail;
    if(!bc)
    {
        bc = bufferchain_new(datasize);
        wb->head = wb->tail = bc;
    }

    // the remain buffer size
    int remain = bc->size - bc->pos - bc->length;

    // if remain buffer size is enough to load the new data
    if(remain >= datasize)
    {
        memcpy(bc->buffer + bc->pos + bc->length, data, datasize);
        bc->length += datasize;
        wb->length += datasize;
    }
    // the remain buffer size is not enough for the new data
    // [1]move the pos ahead to get more space, if enough, the store
    // [2]if not enough, the new a node
    // [2.1]use the remain to store one part
    // [2.2]new a node to store the rest of part
    else if(bufferchain_ismoveahead(bc, datasize))
    {
        bufferchain_moveahead(bc);
        memcpy(bc->buffer + bc->pos + bc->length, data, datasize);
        bc->length += datasize;
        wb->length += datasize;
    }
    else
    {
        if(remain > 0)
        {
            memcpy(bc->buffer + bc->pos + bc->length, data, remain);
            bc->length += remain;
            wb->length += remain;
            datasize -= remain;
        }
        struct bufferchain* tmpnode = bufferchain_new(datasize);
        memcpy(tmpnode->buffer, (uint8_t*)data + remain, datasize);
        bc->next = tmpnode;
        wb->tail = tmpnode;
        tmpnode->length += datasize;
        wb->length += datasize;
    }

    return 0;
}

int wbuffer_write(struct wbuffer* wb, int fd, int datasize)
{
    // log_debug("wbuffer_write ......\n");
    // datasize have been writen to fd
    int writensize = 0;

    // maybe the write buffer has not storen enough data
    if(datasize > wb->length)
    {
        datasize = wb->length;
    }

    // write to fd from head of chain(FIFO)
    struct bufferchain* bc = wb->head;

    while( (datasize > 0) && (bc))
    {
        //XXX: if bc->length > datasize
        int bclength = bc->length;
        if(datasize < bclength)
        {
            bclength = datasize;
        }
        // log_debug("write to fd %d - data : %s\n", fd, bc->buffer + bc->pos);
        int n = write(fd, bc->buffer + bc->pos, bclength);
        if(n < 0)
        {
            // log_error("write to %d error. errno %d", fd, errno);
            switch(errno)
            {
                // INTERRUPT
                // write is block function, but fd is nonblock
                // when write has nothing return, system will return EINTR errno
                case EINTR:
                    continue;
                // nonblock system has nonting to do, let user try again
                case EAGAIN:
                    return writensize;
            }
            return -1;
        }
        writensize += n;
        datasize -= n;
        wb->length -= n;
        // if write datasize equal to head node's data length
        // then use the next node's data
        if(n == bc->length)
        {
            struct bufferchain* tmpnode = bc->next;
            bufferchain_delete(&bc);
            bc = tmpnode;
            wb->head = bc;
            if(!bc)
            {
                wb->tail = NULL;
            }
        }
        else
        {
            // the kernel maybe full
            bc->length -= n;
            bc->pos += n;
            break;
        }
    }


    return writensize;
}

// set write buffer headszie, and ensure use internet little terminal style
// an integer is divisioned into character
int wbuffer_setheader(struct wbuffer* wb, int sz, int headsize)
{
    uint8_t plen[4];
    // ATTENTION:
    // here support the sz with two style: 2 or 4
    // but while using it, the application layer only have the only one, choice 2 as its header, or 4
    if(sz == 2)
    {
        plen[0] = (headsize >> 8) & 0xFF;
        plen[1] = (headsize) & 0xFF;
    }
    else
    {
        plen[0] = (headsize >> 24) & 0xFF;
        plen[1] = (headsize >> 16) & 0xFF;
        plen[2] = (headsize >> 8) & 0xFF;
        plen[3] = (headsize) & 0xFF;
    }

    return wbuffer_pushback(wb, plen, sz);
}

// get write buffer length
int wbuffer_length(struct wbuffer* wb)
{
    return wb->length;
}

// check write buffer is empty
int wbuffer_empty(struct wbuffer* wb)
{
    return wb->head == NULL;
}

// clear the write buffer, then it could be reused, do not need to be malloced again
int wbuffer_clear(struct wbuffer** pwb)
{
    struct bufferchain* bc = (*pwb)->head;
    while(bc)
    {
        bc->length = 0;
        bc->pos = 0;
        memset(bc->buffer, 0, bc->size);
        bc = bc->next;
    }

    return 0;
}


// read buffer chain
struct rbuffer
{
    struct bufferchain* head;
    struct bufferchain* tail;
    int length;
    int header; // TODO:what's the point of header
    // header as a temporary variable, save the buffer's header size
    // sample:
    // the buffer has header data 100, but the buffer data's length is 50
    // will save 100 to header, next time, the buffer data's length is 120
    // then read data from buffer
};

// new a read buffer chain
struct rbuffer* rbuffer_new()
{
    struct rbuffer* rb = (struct rbuffer*)malloc(sizeof(struct rbuffer));
    rb->head = rb->tail = NULL;
    rb->length = 0;
    rb->header = 0;

    return rb;
}

// destroy a read buffer chain
void rbuffer_delete(struct rbuffer** prb)
{
    struct bufferchain* bc = (*prb)->head;
    while(bc)
    {
        struct bufferchain* tmp = bc->next;
        bufferchain_delete(&bc);
        bc = tmp;
    }

    (*prb) = NULL;
}

// read buffer and add to buffer chain from fd
int rbuffer_read(struct rbuffer* rb, int fd, int datasize)
{
    struct bufferchain* bc = rb->head;
    // if the chain is empty, then create
    if(!bc)
    {
        // ATTENTION : the new chain node's size will be given as datasize
        bc = bufferchain_new(datasize);
        rb->head = rb->tail = bc;
    }

    // remain buffer size
    int remain = bc->size - bc->pos - bc->length;
    // situation
    // [1] if remain >= datasize then read
    // [2] if remain < datasize and could be moved ahead, then move ahead and read
    // [3] if could not be moved ahead, then new a next node and read remain size
    if(remain >= datasize)
    {
        // read datasize
    }
    else if((remain < datasize) && bufferchain_ismoveahead(bc, datasize))
    {
        bufferchain_moveahead(bc);
        remain = bc->size - bc->pos - bc->length;
        if(remain >= datasize)
        {
            // read datasize
        }
        else
        {
            // read remain size
            datasize = remain;
        }
    }
    // else if((remain < datasize) && !bufferchain_ismoveahead(bc, datasize))
    else
    {
        // new a node
        struct bufferchain* tmp = bufferchain_new(datasize);
        // tail next
        // bc   tmp
        rb->tail->next = tmp;
        //     tail
        //bc   tmp
        rb->tail = tmp;

        if(remain == 0)
        {
            // read to new node
            bc = tmp;
        }
        else
        {
            // read to the old node and use remain size
            // read remain size, the next data wait for next time call this function
            datasize = remain;
        }

    }

    int readsize = 0;
    readsize = read(fd, bc->buffer + bc->pos + bc->length, datasize);
    // log_debug("recv from fd : %d, readsize : %d, data : %s\n", fd, readsize, bc->buffer + bc->pos + bc->length);
    if(readsize > 0)
    {
        bc->length += readsize;
        rb->length += readsize;
        return readsize;
    }
    else if(readsize < 0)
    {
        switch(errno)
        {
            case EINTR:
                break;
            case EAGAIN:
                log_error("EAGAIN capture, read next time");
                break;
            default:
                log_error("error = %s", strerror(errno));
                return -1;// other error
        }
    }
    else if(readsize == 0)
    {
        return -2; // fd closed
    }

    return -1;
}

// pop buffer from read buffer chain
int rbuffer_popfront(struct rbuffer* rb, void* data, int datasize)
{
    // whether buffer has enough datasize for popping
    if(datasize > rb->length)
    {
        return -1; // doesn't have enough size
    }

    // reduce buffersize
    rb->length -= datasize;

    struct bufferchain* bc = rb->head;
    while((datasize > 0) && bc)
    {
        int buffersize = bc->length;
        // whether datasize < bc->length
        if(datasize < buffersize)
        {
            buffersize = datasize;
        }
        memcpy(data, bc->buffer + bc->pos, buffersize);
        // move pointer ahead
        data = ((char*)data) + buffersize;
        // whether pop all the buffer data from bc
        if(buffersize == bc->length)
        {
            // delete the head
            struct bufferchain* tmp = bc->next;
            bufferchain_delete(&bc);
            bc = tmp;
        }
        else
        {
            // change the buffer idx
            bc->pos += buffersize;
            bc->length -= buffersize;
        }
        datasize -= buffersize;
    }
    rb->head = bc;
    if(!bc)
    {
        // because the tail save the last node
        rb->tail = NULL;
    }

    return 0;
}

// read buffer header
// buffer
// header+buffer
// 10HelloWorld
int rbuffer_getheader(struct rbuffer* rb, int headersize)
{
    // if the first time to read the header
    // log_debug("headersize : %d, rb->length : %d, rb->header : %d\n", headersize, rb->length, rb->header);
    if(rb->header == 0)
    {
        // log_debug("rb->header == 0\n");
        // if the header doesn't exist
        if(rb->length < headersize)
        {
            return -1;// can not be readed
        }
        uint8_t plen[4];
        rbuffer_popfront(rb, plen, headersize);
        if(headersize == 2)
        {
            // bit operation as wbuffer_setheadsize
            rb->header = (plen[0] << 8) | plen[1];
        }
        else
        {
            rb->header = (plen[0] << 24) | (plen[1] << 16) | (plen[2] << 8) | plen[3];
        }
    }
    // if the read buffer size < header, means that there is not enough data for popping
    if(rb->length < rb->header)
    {
        return -1;
    }

    // there is enough data for popping
    int header = rb->header;
    // reset the size of header
    rb->header = 0;

    // log_debug("AAAAAAA get header %d\n", header);

    return header;
}

// read buffer is empty or not
int rbuffer_empty(struct rbuffer* rb)
{
    return rb->head == NULL;
}
// read buffer length
int rbuffer_length(struct rbuffer* rb)
{
    return rb->length;
}

// clear the read buffer, for using it again
int rbuffer_clear(struct rbuffer** prb)
{
    struct bufferchain* bc = (*prb)->head;
    while(bc)
    {
        bc->length = 0;
        bc->pos = 0;
        memset(bc->buffer, 0, bc->size);
        bc = bc->next;
    }

    return 0;
}



// dump
static void buffer_dump(struct bufferchain* bc, const char* name, int length)
{
    log_debug("name = %s,length = %d:\n", name, length);
    while(bc)
    {
        int i = 0;
        for(; i < bc->length; ++i)
        {
            log_debug("%02X ", *(bc->buffer + bc->pos + i));
        }
        log_debug("\n");
        bc = bc->next;
    }
}

// print buffer data
static void buffer_print(struct bufferchain* bc, const char* name, int length)
{
    log_debug("name = %s,length = %d:\n", name, length);
    while(bc)
    {
        log_debug("\tsize=%d,pos=%d,length=%d,data=", bc->size, bc->pos, bc->length);
        int i = 0;
        for(; i < bc->length; ++i)
        {
            log_debug("%c ", *((char*)(bc->buffer + bc->pos + i)));
        }
        log_debug("\n");
        bc = bc->next;
    }
}


// debug
// write buffer dump
void debug_wbuffer_dump(struct wbuffer* wb, const char* name)
{
    buffer_dump(wb->head, name, wb->length);
}

// write buffer print
void debug_wbuffer_print(struct wbuffer* wb, const char* name)
{
    buffer_print(wb->head, name, wb->length);
}

// read buffer dump
void debug_rbuffer_dump(struct rbuffer* rb, const char* name)
{
    buffer_dump(rb->head, name, rb->length);
}

// read buffer print
void debug_rbuffer_print(struct rbuffer* rb, const char* name)
{
    buffer_print(rb->head, name, rb->length);
}





































