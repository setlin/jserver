#include "handler.h"
#include "log.h"
#include "prot.h"

#include <stdlib.h>

void onaccept(struct net_pool* np, int listenfd, int clientfd)
{
    log_debug("onaccept: listenfd : %d, clientfd : %d\n", listenfd, clientfd);;
}

void onconnected(struct net_pool* np, int id)
{
    log_debug("connect gateway successful\n");
    net_set_gatewayid(np, id);
    // send prot 100
    prot_sendprot100(np);
}

void onclose(struct net_pool* np, int id)
{
    log_debug("onclose : id : %d\n", id);
}

void onerror(struct net_pool* np, int id, int what)
{
    log_debug("onerror: id : %d, what : %d\n", id, what);
}

void onrecv(struct net_pool* np, int id, int n)
{
    // step 1 : recv total header
    // step 2 : recv total data
    // step 3 : call prot process handler
    log_debug("onrecv id : %d, n : %d\n", id, n);
    int header = net_recv_header(np, id);

    if(header > 0)
    {
        prot_dispatch(np, id, header);
    }
}

void onsend(struct net_pool* np, int id, int n)
{
    log_debug("onsend : id : %d, size : %d\n", id, n);
}

void reghandler(struct net_pool* np)
{
    // set callback function, by user
    net_set_onaccept(np, onaccept);
    net_set_onconnected(np, onconnected);
    net_set_onclose(np, onclose);
    net_set_onerror(np, onerror);
    net_set_onrecv(np, onrecv);
    net_set_onsend(np, onsend);
}
