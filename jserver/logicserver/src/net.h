/*=============================================================================
#     FileName: net.h
#         Desc: net process
#       Author: Setlin
#      Version: 0.0.1
#       Create: 14:23 2015/03/22
#   LastChange: 17:22 2015/03/27
#      History:
=============================================================================*/
#ifndef _NET_H_
#define _NET_H_

#include "log.h"
#include "epoll.h"
#include "buffer.h"

#include <sys/types.h>
#include <sys/socket.h> // socket
#include <arpa/inet.h> // inet_pton
#include <netinet/in.h> // struct sockaddr_in
#include <netdb.h> // getaddrinfo
#include <stdlib.h> // malloc
#include <fcntl.h> // close
#include <assert.h> // assert
#include <errno.h>
#include <unistd.h>
#include <string.h> // memset
#include <strings.h> // bzero
#include <signal.h> // signal

#include <map>
#include <vector>
#include <string>
#include <list>
#include <algorithm>

using std::map;
using std::vector;
using std::pair;
using std::list;

// some macro deninition
// the minimum size of read once from fd
#define MIN_READ_BUFFER 64
#define MAX_EVENT 32
#define DEFAULT_SOCKET 2 // only connect to gateway
#define MAX_INFO 128 // some information
#define DEFAULT_BACKLOG 32 // listen(fd, backlog) backlog defines the maximum length sockfd grow
#define DEFAULT_HEAD_SIZE 4 // the default head size

#define DEFAULT_PROT_SIZE 16*1024*1024
#define DEFAULT_PROT_SIZE_SMALL 1024

// timer slot interval
#define DEFAULT_TIMERSLOT 1000

// status of sockfd
#define SOCKET_STATUS_INVALID    0
#define SOCKET_STATUS_RESERVE    1
#define SOCKET_STATUS_LISTEN     2
#define SOCKET_STATUS_CONNECTING 3
#define SOCKET_STATUS_CONNECTED  4
#define SOCKET_STATUS_SIGNAL     5

// store sql
typedef list<char*> SQLLIST;

// net pool
// save the connected clients's datas
// net pool, reserve clientfd-id
struct net_pool;

// callback function
// function pointer
// accept function
// do not use listenfd to mark the who is listenning, for the fd will be reused
// but the increasing id is the unique one
typedef void (*ONACCEPT)(struct net_pool* np, int listenid, int clientid);
// connect function, for client
// when the client is connected, and got its fd and increasing id, then link the id to np
typedef void (*ONCONNECTED)(struct net_pool* np, int id);
// close function
typedef void (*ONCLOSE)(struct net_pool* np, int id);
// on error
// net_pool, fd-id, errno
typedef void (*ONERROR)(struct net_pool* np, int id, int what);
// on recv
// the recv callback function will handler the specific protid
// when recv the data, according the header, will get the prot data
// prot data include: protheader,protdata
// sample:
// recvdata(a data pack): header,data [9,[1,1,Hello]], header is data length
// protdata: header, data [[1,1],Hello], header is [protid, charid]
typedef void (*ONRECV)(struct net_pool* np, int id, int n);
// on send
typedef void (*ONSEND)(struct net_pool* np, int id, int n);

// on dispatch protocols
typedef void (*ONDISPATCHPROT)();


// set callback function
// packaging the element of struct is safaty for using
// the elements can not be known by users
void net_set_onaccept(struct net_pool* np, ONACCEPT onaccept);
void net_set_onconnected(struct net_pool* np, ONCONNECTED onconnected);
void net_set_onclose(struct net_pool* np, ONCLOSE onclose);
void net_set_onerror(struct net_pool* np, ONERROR onerror);
void net_set_onrecv(struct net_pool* np, ONRECV onrecv);
void net_set_onsend(struct net_pool* np, ONSEND onsend);
void net_set_gatewayid(struct net_pool* np, int gatewayid);
int net_get_gatewayid(struct net_pool* np);



// new a net pool
struct net_pool* net_new();
// delete a net net pool
void net_delete(struct net_pool** pnp);
// set user data for lua
void net_set_userdata(struct net_pool* np, void* ud);
// get user data for lua
void* net_get_userdata(struct net_pool* np);


// connect, for client
// net pool, ip address, port
int net_connect(struct net_pool* np, const char* host, int port);
// listen, for server
int net_listen(struct net_pool* np, char* host, int port);
// send header(header data, and header's size)
// net pool, selfadd id(unique mark for sockfd,client)
// unique id mapping with sockfd, so mark the unique client
int net_send_header(struct net_pool* np, int id, int sz);
// send data to client
// net pool, unique id, data, datasize
int net_send(struct net_pool* np, int id, const uint8_t* data, int sz);
// the loop in the main thread
int net_loop(struct net_pool* np, int timeout);
// recv header
// user level must define the header size, but the buffer level must support diffirent header size
int net_recv_header(struct net_pool* np, int id);
// recv data
// the real one recv data, will be used by callback function like following ONRECV
int net_recv(struct net_pool* np, int id, void* data, int sz);

// protocol dispatch hoandler
typedef int (*PROTHANDLER)(struct net_pool* np, int connid, int seqid, int charid, uint8_t* data, int datalength);
// register protocol handler
void NET_REG_PROTHANDLER(struct net_pool* np, int protid, PROTHANDLER prothandler);
// map iterator
typedef map<int,PROTHANDLER>::iterator PROTHANDLERITER;
// get prothander
PROTHANDLER NET_GET_PROTHANDLER(struct net_pool* np, int protid);


// clear sqllist
int net_sqllist_clear(struct net_pool* np);
// get the sqllist
int net_sqllist_get(struct net_pool* np, SQLLIST* psqllist);
// set sqllist
int net_sqllist_set(struct net_pool* np, char* szsql);


#endif



















