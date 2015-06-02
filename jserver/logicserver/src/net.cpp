#include "net.h"
#include "jslogdb.h"

// global value
// signal pipe fd
static int sigpipefd[2];

// for compatibility
union sockaddr_all
{
    struct sockaddr s;
    struct sockaddr_in v4; // use ipv4
    struct sockaddr_in6 v6; // use ipv6
};

// unique selfadd id
// sample
// client1 -> gateway sockfd,id(1001, 0)
// gateway -> server sockfd,id(1001,0)
// when server process data, client1 close, and (1001,0) is invalid
// client2 -> gateway sockfd,id(1001, 1)
// when gateway recv processed data from server, (1001, 0), and it is invalid
// will not send to client2
// the id will grow up untill maximum, then reuse
// [attention]if sockfd is invalid, it will be reused


// socket info
struct net_socket
{
    // sockfd
    int fd;
    // unique selfadd id, for marking the relationship between sockfd and charid
    int id;
    // socket status
    int status;
    // default readsize from recv, will recv enough data from clientfd
    int rdsz;
    // write buffer size
    struct wbuffer* wbuff;
    // read buffer size
    struct rbuffer* rbuff;
    // information saves ip address
    char info[MAX_INFO];
};



// net pool
// save the connected clients's datas
// net pool, reserve clientfd-id
struct net_pool
{
    // eventfd, by epoll_create
    event_fd eventfd;
    // event count
    int eventn;
    // current processed event number, < eventn
    int eventindex;
    // the real increasing id, used for net_socket.id
    // means that net_socket.id = allocid
    int allocid;
    // capacity of client's number, will grow up double once
    int cap;
    // current client's number, if count >= cap, then expand the capacity
    int count;
    // events from epoll_wait
    struct event es[MAX_EVENT];
    // client's data, an array, using allocid as its subscript
    struct net_socket **ns;
    // userdata, for lua, load ip address, port
    void* ud;

    // callback function
    ONACCEPT onaccept;
    ONCONNECTED onconnected;
    ONCLOSE onclose;
    ONERROR onerror;
    ONRECV onrecv;
    ONSEND onsend;

    map<int, PROTHANDLER>* prothandler;
    SQLLIST* psqllist;
    int timeout;

    int gatewayid;
};




// ensure the client is alive, just like heatbeat
// but suggest that having a heartbeat function in the application layer, could control the client's sutiation better
// XXX: use a heartbeat function in APP layer instead of system call
static void socket_keepalive(int fd)
{
    int keepalive = 1;
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepalive, sizeof(keepalive));
}

// close fd
static void socket_close(int fd)
{
    close(fd);
}

// signal
// signal handler
static void sig_handler(int sig)
{
    int saveerrno = errno;
    int msg = sig;
    send(sigpipefd[1], (char*)&msg, 1, 0);
    errno = saveerrno;
}

// add signal to handler
static void addsig(int sig)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}



// delete a net net pool
void net_delete(struct net_pool** pnp)
{
    struct net_pool* np = *pnp;
    // the array exist or not
    if(np->ns)
    {
        int i = 0;
        for(; i < np->cap; ++i)
        {
            if(np->ns[i]->status != SOCKET_STATUS_INVALID)
            {
                socket_close(np->ns[i]->fd);
            }
            // TODO: free the buffer
            rbuffer_delete(&(np->ns[i]->rbuff));
            wbuffer_delete(&(np->ns[i]->wbuff));
            free(np->ns[i]);
            np->ns[i] = NULL;
        }

        free(np->ns);
        np->ns = NULL;
    }

    np->cap = 0;
    np->count = 0;

    if(!poll_invalid(np->eventfd))
    {
        poll_release(np->eventfd);
    }

    np->allocid = 0;
    np->eventfd = 0;
    np->eventindex = 0;
    np->eventn = 0;

    free(*pnp);
    *pnp = NULL;
}

// set user data for lua
void net_set_userdata(struct net_pool* np, void* ud)
{
    np->ud = ud;
}

// get user data for lua
void* net_get_userdata(struct net_pool* np)
{
    return np->ud;
}

// some functions called in the file
// self increasing id
static int reserve_id(struct net_pool* np)
{
    int i = 0;
    // will get np->cap times loop
    for(; i < np->cap; ++i)
    {
        // get the self increased id
        // just like ++np->allocid;
        // XXX: use __sync_fetch_and_add
        // int id = __sync_add_and_fetch(&(np->allocid), 1);
        // generally, fetch once could get the id
        int id = __sync_fetch_and_add(&(np->allocid), 1);
        // if over or equal to the 2^32, bigger than 4 bits, the number will be a negative
        // then cut down
        // singel bit [-128 - 127]
        // [-2^7] - [2^7-1]
        // -128 : 1000,0000
        // 127  : 0111,1111
        // 127 + 1 = 1000,0000 = -128
        // -128 + 1 = -127 : 1000,0001
        // -1 : 1111,1111 => 1000,0001:1111,1110:1111,1111
        // 2^32 (-2^32) 0x80000000    1000,0000,0000,0000,0000,0000,0000,0000 (32bits)
        // 2^31  :      0x7FFFFFFF    0111,1111,1111,1111,1111,1111,1111,1111
        //  0x80000000 & 0x7FFFFFFF : 0
        if(id < 0)
        {
            id = __sync_and_and_fetch(&(np->allocid), 0x7FFFFFFF);
        }

        // incase of spilling
        // reuse id
        struct net_socket* ns = np->ns[id % np->cap];
        log_debug("reserve - index : %d, cap : %d, ns->status : %d\n", id % np->cap, np->cap, ns->status);
        // if ns->status != SOCKET_STATUS_INVALID, then go to next loop
        if(ns->status == SOCKET_STATUS_INVALID)
        {
            // if ns->status == SOCKET_STATUS_INVALID, then ns->status = SOCKET_STATUS_RESERVE
            // incase of other thread use the id in the same moment
            if(__sync_bool_compare_and_swap(&(ns->status), SOCKET_STATUS_INVALID, SOCKET_STATUS_RESERVE))
            {
                ns->id = id;
                ns->fd = -1;
                log_debug("reserve id %d \n", id);
                return id;
            }
            else
            {
                --i;
            }
        }
    }

    return -1;
}

// expand the capacity
static void net_expand(struct net_pool* np)
{
    log_debug("net_expand ...\n");
    int newcap = np->cap * 2;

    // new array for saving net socket's information
    struct net_socket** ns = (struct net_socket**)malloc(newcap * sizeof(struct net_socket*));
    memset(ns, 0, newcap * sizeof(struct net_socket*));

    int i = 0;
    // transplant the old data to new container
    for(; i < np->cap; ++i)
    {
        // in fact, np->ns[i]->id == i
        // use %, incase of spilling
        int nid = np->ns[i]->id % newcap;
        if(ns[nid] != NULL)
        {
            log_error("new net_socket error : %d", i);
            continue;
        }
        ns[nid] = np->ns[i];
    }

    // malloc new container's data
    for(i = 0; i < newcap; ++i)
    {
        if(!ns[i])
        {
            ns[i] = (struct net_socket*)malloc(sizeof(struct net_socket));
            memset(ns[i], 0, sizeof(struct net_socket));
            ns[i]->rdsz = MIN_READ_BUFFER;
            ns[i]->rbuff = rbuffer_new();
            ns[i]->wbuff = wbuffer_new();
        }
    }

    // free the old array
    free(np->ns);
    // set the new array
    np->ns = ns;
    // increse the cap
    np->cap = newcap;
}

// new a net socket node
// the add parametic means that let fd add to eventfd immediately or not
static struct net_socket* net_socket_new(struct net_pool* np, int fd, bool add)
{
    // judge the capacity is full or not
    if(np->count >= np->cap)
    {
        net_expand(np);
    }

    // get the unique id for fd
    int id = reserve_id(np);
    if(id < 0)
    {
        log_error("reserve id failure");
        return NULL;
    }

    struct net_socket* ns = np->ns[id % np->cap];
    // TODO: use assert?
    // assert(ns->status == SOCKET_STATUS_RESERVE);
    if(ns->status != SOCKET_STATUS_RESERVE)
    {
        log_error("get allocid failure");
        return NULL;
    }

    // add to eventfd or not
    if(add)
    {
        if(poll_add(np->eventfd, fd, ns))
        {
            log_error("add fd to eventfd failure");
            ns->status = SOCKET_STATUS_INVALID;
            return NULL;
        }
    }
    ns->fd = fd;
    np->count++;

    return ns;
}

// set signal to np
static void net_set_sighandler(struct net_pool* np)
{
    // add signal handler
    int status = socketpair(PF_UNIX, SOCK_STREAM, 0, sigpipefd);
    assert(status != -1);

    poll_setnonblocking(sigpipefd[1]);
    struct net_socket* nstmp = net_socket_new(np, sigpipefd[0], true);

    nstmp->status = SOCKET_STATUS_SIGNAL;

    addsig(SIGTERM);
    addsig(SIGINT);
    addsig(SIGALRM);

    alarm(DEFAULT_TIMERSLOT);
}

// new a net pool
struct net_pool* net_new()
{
    struct net_pool* np = (struct net_pool*)malloc(sizeof(struct net_pool));

    // get the eventfd
    np->eventfd = poll_create();
    if(poll_invalid(np->eventfd))
    {
        log_error("epoll create failure\n");
        return NULL;
    }

    np->eventn = 0;
    np->eventindex = 0;
    np->allocid = 0;
    np->count = 0;
    np->cap = DEFAULT_SOCKET;

    // beforehand alloc an array as a container
    np->ns = (struct net_socket**)malloc(np->cap * sizeof(struct net_socket*));

    // initialize the elements of array: alloc memory
    int i = 0;
    for(; i < np->cap; ++i)
    {
        np->ns[i] = (struct net_socket*)malloc(sizeof(struct net_socket));
        memset(np->ns[i], 0, sizeof(struct net_socket));
        np->ns[i]->rdsz = MIN_READ_BUFFER;
        np->ns[i]->wbuff = wbuffer_new();
        np->ns[i]->rbuff = rbuffer_new();
    }

    np->ud = NULL;


    // set signal to np
    net_set_sighandler(np);

    // np->prothandler = (map<int, PROTHANDLER>*)malloc(sizeof(map<int, PROTHANDLER>)) ;
    // map is a type of class, use new, call structure function

    np->prothandler = new map<int, PROTHANDLER>;
    np->gatewayid = -1;

    np->timeout = 0;

    np->psqllist = new SQLLIST;

    return np;
}

// connect, for client
// net pool, ip address, port
int net_connect(struct net_pool* np, const char* host, int port)
{
    log_debug("connect : host(ipaddress) = %s, port = %d\n", host, port);

    // hint the system what do here need
    struct addrinfo ai_hints;
    // reserve the result chain
    struct addrinfo* ai_list = NULL;
    // as the element of result chain
    struct addrinfo* ai_ptr;

    char strport[16];
    memset(strport, 0, 16);
    sprintf(strport, "%d", port);

    // set hint information
    memset(&ai_hints, 0, sizeof(struct addrinfo));
    ai_hints.ai_family = AF_UNSPEC; // point that sin_family is unspecified, other choose : AF_INET, AF_INET6
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_protocol = IPPROTO_TCP;

    int status = getaddrinfo(host, strport, &ai_hints, &ai_list);
    if(status != 0)
    {
        freeaddrinfo(ai_list);
        return -1;
    }

    // the sockfd, connected to server
    int fd = -1;
    for(ai_ptr =ai_list; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next)
    {
        fd = socket(ai_ptr->ai_family, ai_ptr->ai_socktype, ai_ptr->ai_protocol);
        if(fd < 0)
        {
            log_error("socket failure ...");
            continue;
        }

        socket_keepalive(fd);
        poll_setnonblocking(fd);

        // connect to server
        status = connect(fd, ai_ptr->ai_addr, ai_ptr->ai_addrlen);
        if((status != 0) && (errno != EINPROGRESS))
        {
            socket_close(fd);
            fd = -1;
            continue;
        }
        break;
    }

    if(fd < 0)
    {
        freeaddrinfo(ai_list);
        return -1;
    }

    struct net_socket* ns = net_socket_new(np, fd, true);
    if(ns == NULL)
    {
        socket_close(fd);
        freeaddrinfo(ai_list);
        return -1;
    }

    if(status == 0)
    {
        ns->status = SOCKET_STATUS_CONNECTED;
        struct sockaddr* addr = ai_ptr->ai_addr;
        void* sin_addr = (ai_ptr->ai_family == AF_INET)
                        ? ((void*)&((struct sockaddr_in*)addr)->sin_addr)
                        : ((void*)&((struct sockaddr_in6*)addr)->sin6_addr);

        // save ip address to ns->info
        inet_ntop(ai_ptr->ai_family, sin_addr, ns->info, sizeof(ns->info));
        log_info("connected immediately id = %d\n", ns->id);
        np->onconnected(np, ns->id);
    }
    else
    {
        ns->status = SOCKET_STATUS_CONNECTING;
        poll_write(np->eventfd, ns->fd, ns, true);
    }

    freeaddrinfo(ai_list);

    return ns->id;
}

// listen, for server
static int dolisten(char* host, int port, int backlog)
{
    struct addrinfo ai_hints;
    struct addrinfo* ai_list = NULL;
    memset(&ai_hints, 0, sizeof(struct addrinfo));

    char strport[16];
    memset(strport, 0, 16);
    if(host == NULL || host[0] == 0)
    {
        // local host
        sprintf(host, "%s", "0.0.0.0");
    }

    sprintf(strport, "%d", port);

    ai_hints.ai_family = AF_UNSPEC;
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_protocol = IPPROTO_TCP;

    int status = getaddrinfo(host, strport, &ai_hints, &ai_list);
    if(status != 0)
    {
        return -1;
    }

    int listenfd = socket(ai_list->ai_family, ai_list->ai_socktype, 0);
    if(listenfd < 0)
    {
        log_error("socket error");
        freeaddrinfo(ai_list);
        return -1;
    }

    // set port reuse
    int reuse = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse, sizeof(int)) == -1)
    {
        socket_close(listenfd);
        freeaddrinfo(ai_list);
        log_error("setsockopt error");
        return -1;
    }

    // bind fd and port/address
    status = bind(listenfd, (struct sockaddr*)ai_list->ai_addr, ai_list->ai_addrlen);
    if(status != 0)
    {
        socket_close(listenfd);
        freeaddrinfo(ai_list);
        log_error("bind error");
        return -1;
    }

    freeaddrinfo(ai_list);

    // listen
    if(listen(listenfd, backlog))
    {
        socket_close(listenfd);
        log_error("listen error");
        return -1;
    }

    return listenfd;
}


// listen, for server
int net_listen(struct net_pool* np, char* host, int port)
{
    log_debug("listen: host = %s, port = %d\n", host, port);

    int listenfd = dolisten(host, port, DEFAULT_BACKLOG);

    if(listenfd < 0)
    {
        return -1;
    }

    struct net_socket* ns = net_socket_new(np, listenfd, true);
    if(ns == NULL)
    {
        socket_close(listenfd);
        log_error("listen failure");
        return -1;
    }

    ns->status = SOCKET_STATUS_LISTEN;

    return ns->id;
}

// send header(header data, and header's size)
// net pool, selfadd id(unique mark for sockfd,client)
// unique id mapping with sockfd, so mark the unique client
int net_send_header(struct net_pool* np, int id, int sz)
{
    log_debug("net_send_header headsize : %d\n", sz);
    struct net_socket* ns = np->ns[id % np->cap];
    wbuffer_setheader(ns->wbuff, DEFAULT_HEAD_SIZE, sz);
    poll_write(np->eventfd, ns->fd, ns, true);

    return 0;
}

// send data to client
// net pool, unique id, data, datasize
int net_send(struct net_pool* np, int id, const uint8_t* data, int sz)
{
    struct net_socket* ns = np->ns[id % np->cap];
    wbuffer_pushback(ns->wbuff, data, sz);
    poll_write(np->eventfd, ns->fd, ns, true);

    return 0;
}

// recv header
// user level must define the header size, but the buffer level must support diffirent header size
int net_recv_header(struct net_pool* np, int id)
{
    struct net_socket* ns = np->ns[id % np->cap];
    return rbuffer_getheader(ns->rbuff, DEFAULT_HEAD_SIZE);
}

// recv data
// the real one recv data, will be used by callback function like following ONRECV
int net_recv(struct net_pool* np, int id, void* data, int sz)
{
    struct net_socket* ns = np->ns[id % np->cap];
    return rbuffer_popfront(ns->rbuff, data, sz);
}

// force close socket fd
static void net_force_close(struct net_pool* np, struct net_socket* ns)
{
    log_debug("net_force_close ...\n");
    // FIXME: could not be delete, the buffer would be reused again
    // wbuffer_delete(&(ns->wbuff));
    // rbuffer_delete(&(ns->rbuff));
    // clear the buffer
    wbuffer_clear(&(ns->wbuff));
    rbuffer_clear(&(ns->rbuff));

    ns->status = SOCKET_STATUS_INVALID;
    if(ns->fd > 0)
    {
        poll_del(np->eventfd, ns->fd);
        socket_close(ns->fd);
        ns->fd = -1;
    }

    // reduce the count of client
    --np->count;
    np->onclose(np, ns->fd);
    delete np->prothandler;
    np->gatewayid = -1;

    // delete np->psqllist;
}

static int net_socket_report_connect(struct net_pool* np, struct net_socket* ns)
{
    int error;
    socklen_t len = sizeof(int);

    // get the socketfd's option
    int code = getsockopt(ns->fd, SOL_SOCKET, SO_ERROR, &error, &len);
    if((code < 0) || error)
    {
        net_force_close(np, ns);
        np->onerror(np, ns->id, 1); // report connect error
        log_error("connect %d", ns->id);
        return -1;// getsockopt error
    }

    ns->status = SOCKET_STATUS_CONNECTED;
    if(wbuffer_empty(ns->wbuff))
    {
        poll_write(np->eventfd, ns->fd, ns, false);
    }

    union sockaddr_all u;
    socklen_t slen = sizeof(u);
    if(getpeername(ns->fd, &u.s, &slen) == 0)
    {
        void* sin_addr = (u.s.sa_family == AF_INET)
                        ? (void*)&u.v4.sin_addr
                        : (void*)&u.v6.sin6_addr;
        inet_ntop(u.s.sa_family, sin_addr, ns->info, sizeof(ns->info));
    }

    np->onconnected(np, ns->id);

    return 0;
}

// on reprot accept, for server
// ns is the struct of server
static int net_socket_report_accept(struct net_pool* np, struct net_socket* ns)
{
    log_debug("net_socket_report_accept ...\n");
    union sockaddr_all u;
    socklen_t len = sizeof(u);

    int clientfd = accept(ns->fd, &u.s, &len);
    if(clientfd < 0)
    {
        np->onerror(np, ns->id, 2); // accept error
        log_error("accept failure %d", ns->id);
        return -1;
    }

    socket_keepalive(clientfd);
    poll_setnonblocking(clientfd);

    // when accept a new client, need to judge the count of online clients is reach the limit
    // so do not add the new clientfd to eventfd
    struct net_socket* nstmp = net_socket_new(np, clientfd, false);
    if(nstmp == NULL)
    {
        socket_close(clientfd);
        np->onerror(np, ns->id, 3);// new socket failure
        log_error("new socket error %d", ns->id);
        return -2; // new socket error
    }

    // TODO: do some judgement, such as the online number limit

    // add clientfd to eventfd
    if(poll_add(np->eventfd, clientfd, nstmp))
    {
        socket_close(clientfd);
        nstmp->status = SOCKET_STATUS_INVALID;
        np->onerror(np, ns->id, 4); // add to eventfd failure
        log_error("report accept , poll add");
        return -3;
    }

    nstmp->status = SOCKET_STATUS_CONNECTED;

    void* sin_addr = (u.s.sa_family == AF_INET)
                    ?((void*)&u.v4.sin_addr)
                    :((void*)&u.v6.sin6_addr);

    inet_ntop(u.s.sa_family, sin_addr, nstmp->info, sizeof(nstmp->info));

    np->onaccept(np, ns->id, nstmp->id);

    log_debug("the new client fd : %d\n", clientfd);

    return 0;
}

// send buffer
static int net_socket_send_buffer(struct net_pool* np, struct net_socket* ns)
{
    log_debug("net_socket_send_buffer ...\n");
    int wsz = wbuffer_write(ns->wbuff, ns->fd, wbuffer_length(ns->wbuff));
    if(wsz < 0)
    {
        net_force_close(np, ns);
        np->onerror(np, ns->id, 5); // send error
        log_error("wsz < 0, id = %d", ns->id);
        return -1;
    }
    if(wbuffer_length(ns->wbuff) == 0)
    {
        poll_write(np->eventfd, ns->fd, ns, false);
    }

    np->onsend(np, ns->id, wsz);

    return wsz;
}

// recv buffer
static int net_socket_recv_buffer(struct net_pool* np, struct net_socket* ns)
{
    log_debug("net_socket_recv_buffer, fd : %d, rdsz : %d \n", ns->fd, ns->rdsz);

    int n = rbuffer_read(ns->rbuff, ns->fd, ns->rdsz);
    log_debug("net_socket_recv_buffer : data : %s\n", ns->rbuff);
    log_debug("recv size %d\n", n);
    if(n < 0)
    {
        if(n == -2)
        {
            log_debug("client leave %d\n", ns->fd);
            net_force_close(np, ns);
            return -1;
        }
        else
        {
            log_error("recv error : %d", ns->id);
            np->onerror(np, ns->id, 6);
            return -1;
        }
    }

    // increase or reduce the recv size
    if(n == ns->rdsz)
    {
        ns->rdsz = ns->rdsz * 2;
    }
    else if((ns->rdsz > MIN_READ_BUFFER) && (ns->rdsz > n * 2))
    {
        ns->rdsz = ns->rdsz / 2;
    }

    log_debug("net_socket_recv_buffer rbuffer_read : %d\n", n);
    np->onrecv(np, ns->id, n);

    return n;
}

static void timerhandler(struct net_pool* np)
{
    log_debug("timerhandler ...\n");

    /*
    char* szSql = (char*)malloc(1024);
    memset(szSql, 0, 1024);

    sprintf(szSql, "%s", "UPDATE jscharinfo SET charname='chenjielin' where charid = 1002");

    np->psqllist->push_back(szSql);
    */
    JSLogDBByFork(np);

    alarm(DEFAULT_TIMERSLOT);
}

// process signal
static int net_socket_report_signal(struct net_pool* np, struct net_socket* ns)
{
    int status = 0;

    char signals[1024];
    status = recv(sigpipefd[0], signals, sizeof(signals), 0);
    if(status == -1)
    {
        log_error("recv from sigpipefd[0] failure");
        return status;
    }

    int i = 0;
    for(; i < status; ++i)
    {
        switch(signals[i])
        {
            case SIGTERM:
            case SIGINT:
                log_debug("[SHUTDOWN] ...\n");
                net_delete(&np);
                exit(-1);
                break;
            case SIGALRM:
                np->timeout = 1;
                log_debug("[TIMEUP] ...\n");
                break;
        }
    }

    if(np->timeout == 1)
    {
        timerhandler(np);
        np->timeout = 0;
    }

    return status;
}


// the loop in the main thread
int net_loop(struct net_pool* np, int timeout)
{
    // use for(;;) is better than while(1)
    // because while(1) need to judge the the true condition, but for(;;) do not
    for(;;)
    {
        // while there is nothing to process, then wait
        if(np->eventindex == np->eventn)
        {
            np->eventn = poll_wait(np->eventfd, np->es, MAX_EVENT, timeout);
            log_debug("np->eventn : %d\n", np->eventn);
            np->eventindex = 0;
            if(np->eventn <= 0)
            {
                np->eventn= 0;
                return -1;
            }
        }

        // get the first one event to process
        struct event* e = &(np->es[np->eventindex++]);
        struct net_socket* ns = (struct net_socket*)e->data;

        if(ns == NULL)
        {
            continue;
        }

        log_debug("ns->status : %d\n", ns->status);
        switch(ns->status)
        {
            case SOCKET_STATUS_CONNECTING:
                return net_socket_report_connect(np, ns);
                break;
            case SOCKET_STATUS_LISTEN:
                return net_socket_report_accept(np, ns);
                break;
            case SOCKET_STATUS_INVALID:
                log_error("invalid socket fd");
                break;
            case SOCKET_STATUS_SIGNAL:
                return net_socket_report_signal(np, ns);
                break;
            default:
                if(e->iswrite)
                {
                    int ret = net_socket_send_buffer(np, ns);
                    if(ret < 0)
                    {
                        break;
                    }
                    return ret;
                }

                if(e->isread)
                {
                    int ret = net_socket_recv_buffer(np, ns);
                    if(ret < 0)
                    {
                        break;
                    }
                    return ret;
                }
                break;
        }


    }

    log_debugEx("loop once");
    return 0;
}


// set callback function
// packaging the element of struct is safaty for using
// the elements can not be known by users
void net_set_onaccept(struct net_pool* np, ONACCEPT onaccept)
{
    np->onaccept = onaccept;
}

void net_set_onconnected(struct net_pool* np, ONCONNECTED onconnected)
{
    np->onconnected = onconnected;
}

void net_set_onclose(struct net_pool* np, ONCLOSE onclose)
{
    np->onclose = onclose;
}

void net_set_onerror(struct net_pool* np, ONERROR onerror)
{
    np->onerror = onerror;
}

void net_set_onrecv(struct net_pool* np, ONRECV onrecv)
{
    np->onrecv = onrecv;
}

void net_set_onsend(struct net_pool* np, ONSEND onsend)
{
    np->onsend = onsend;
}

void net_set_gatewayid(struct net_pool* np, int gatewayid)
{
    np->gatewayid = gatewayid;
}

int net_get_gatewayid(struct net_pool* np)
{
    return np->gatewayid;
}

// register protocol handler
void NET_REG_PROTHANDLER(struct net_pool* np, int protid, PROTHANDLER prothandler)
{
    log_debug("NET_REG_PROTHANDLER\n");
    log_debug("address %x\n", prothandler);
    np->prothandler->insert(pair<int,PROTHANDLER>(protid, prothandler));
}

// get prothander
PROTHANDLER NET_GET_PROTHANDLER(struct net_pool* np, int protid)
{
    PROTHANDLERITER iter = np->prothandler->find(protid);

    if(iter == np->prothandler->end())
    {
        log_error("protid [%d] handler could not be fould",protid);
        return NULL;
    }

    return iter->second;
}


// clear sqllist
int net_sqllist_clear(struct net_pool* np)
{
    log_debug("net_sqllist_clear\n");
    int res = 0;

    SQLLIST::iterator it = np->psqllist->begin();

    for(; it != np->psqllist->end(); ++it)
    {
        free(*it);
    }

    np->psqllist->clear();

    return res;
}


// get the sqllist
int net_sqllist_get(struct net_pool* np, SQLLIST* psqllist)
{
    log_debug("net_sqllist_get\n");
    int res = 0;

    char* tempSzSQL = np->psqllist->front();
    log_debug("%s\n", tempSzSQL);

    //  psqllist = np->psqllist;
   
    SQLLIST::iterator it = np->psqllist->begin();

    for(; it != np->psqllist->end(); ++it)
    {
        psqllist->push_back(*it);
    }

    return res;
}


// set sqllist
int net_sqllist_set(struct net_pool* np, char* szsql)
{
    int res = 0;

    np->psqllist->push_back(szsql);

    return res;
}

















