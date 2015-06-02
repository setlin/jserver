/*=============================================================================
#     FileName: buffer.h
#         Desc: recv/send data from/to kernel buffer
#       Author: Setlin
#      Version: 0.0.1
#       Create: 10:59 2015/03/22
#   LastChange: 10:59 2015/03/23
#      History:
=============================================================================*/
#ifndef _BUFFER_H_
#define _BUFFER_H_

// write buffer struct
struct wbuffer;

// create a write buffer chain
struct wbuffer* wbuffer_new();
// delete a write buffer chain
void wbuffer_delete(struct wbuffer** pwb);
// pushback a data to write buffer chain
int wbuffer_pushback(struct wbuffer* wb, const void* data, int datasize);
// write data to fd(kernel send buffer area), fd would be full, so parhaps it will send part of datasize
int wbuffer_write(struct wbuffer* wb, int fd, int datasize);
// set write buffer headszie, and ensure use internet little terminal style
// an integer is divisioned into character
// 0xFF = 11111111 = 127 (1 bit)
// sample 1: num = 1024 (use 2 bits) 00000010,00000000
// change into character: str[2]
// str[0] = (num >> 8) & 0xFF = 00000010 & 0xFF = 00000010 = 2
// str[1] = (num) & 0xfFF = 0
// sample 2: num = 65536 (use 4 bit) 00000000,00000001,00000000,00000000
// change into character: str[4]
// str[0] = (num >> 24) & 0xFF = 00000000 & 0xFF = 00000000 = 0
// str[1] = (num >> 16) & 0xFF = 00000000,00000001 & 0xFF = 00000000,00000001 = 1
// str[2] = (num >> 8) & 0xFF = 00000000,00000001,00000000 & 0xFF = 00000000,00000000,00000000 = 0
// str[3] = (num) & 0xFF = 00000000,00000001,00000000,00000000 & 0xFF = 00000000,00000000,00000000,00000000 = 0
int wbuffer_setheader(struct wbuffer* wb, int sz, int headsize);
// get write buffer length
int wbuffer_length(struct wbuffer* wb);
// check write buffer is empty
int wbuffer_empty(struct wbuffer* wb);
// clear the write buffer, then it could be reused, do not need to be malloced again
int wbuffer_clear(struct wbuffer** pwb);


// read buffer struct
struct rbuffer;
// new a read buffer chain
struct rbuffer* rbuffer_new();
// destroy a read buffer chain
void rbuffer_delete(struct rbuffer** prb); // use pp, will modify the chain, p can not modify its value, because chain is p
// read buffer and add to buffer chain from fd
int rbuffer_read(struct rbuffer* rb, int fd, int datasize);
// pop buffer from read buffer chain
int rbuffer_popfront(struct rbuffer* rb, void* data, int datasize);
// read buffer header
// buffer
// header+buffer
// 10HelloWorld
int rbuffer_getheader(struct rbuffer* rb, int headersize);
// read buffer is empty or not
int rbuffer_empty(struct rbuffer* rb);
// read buffer length
int rbuffer_length(struct rbuffer* rb);
// clear the read buffer, for using it again
int rbuffer_clear(struct rbuffer** prb);


// debug
// write buffer dump
void debug_wbuffer_dump(struct wbuffer* wb, const char* name);
// write buffer print
void debug_wbuffer_print(struct wbuffer* wb, const char* name);
// read buffer dump
void debug_rbuffer_dump(struct rbuffer* rb, const char* name);
// read buffer print
void debug_rbuffer_print(struct rbuffer* rb, const char* name);

#endif


































