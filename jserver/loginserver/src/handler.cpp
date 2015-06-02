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
    log_debug("onconnected : id : %d\n", id);
}

void onclose(struct net_pool* np, int id)
{
    log_debug("onclose : id : %d\n", id);
}

void onerror(struct net_pool* np, int id, int what)
{
    log_debug("onerror: id : %d, what : %d\n", id, what);
}

static void ontranspond_sendprot_toserver_setconnid(uint8_t* protdata, int connid)
{
    protdata[0] = (connid >> 24) & 0xFF;
    protdata[1] = (connid >> 16) & 0xFF;
    protdata[2] = (connid >> 8) & 0xFF;
    protdata[3] = (connid ) & 0xFF;
}

static int ontranspond_sendprot_getprotid(uint8_t* protdata)
{
    int protid = 0;
    protid = (protdata[4] << 24)
            | (protdata[5] << 16)
            | (protdata[6] << 8)
            | (protdata[7]);

    return protid;
}

// SendProt API
// gateway can not believe
static int ontranspond_sendprot_toserver(struct net_pool* np, int header, uint8_t* protdata, int connid)
{
    int serverid = net_get_serverid(np);
    net_send_header(np, serverid, header);

    // must use the connid that saved in gateway
    ontranspond_sendprot_toserver_setconnid(protdata, connid);

    net_send(np, serverid, protdata, header);
    return 0;
}

static int ontranspond_sendprot_toclient(struct net_pool* np, int id, int header, uint8_t* protdata)
{
    log_debug("ontranspond_sendprot_toclient ...\n");
    log_debug("header : %d\n", header);
    log_debug("protdata : %s\n", protdata);
    net_send_header(np, id, header);
    net_send(np, id, protdata, header);

    return 0;
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
        uint8_t* protdata = (uint8_t*)malloc(header);
        memset(protdata, 0, header);
        net_recv(np, id, protdata, header);

        int protid = -1;
        protid = ontranspond_sendprot_getprotid(protdata);

        log_debug("XXXXXXXXXXXXXXXX protid : %d\n", protid);

        int serverid = net_get_serverid(np);
        // if server does not connect to gateway then process the protocol
        if(serverid == -1 )
        {
            log_debug("[onrecv] server don't connect\n");
            prot_dispatch(np, id, header, protdata);
        }
        else
        {
            if(protid == PROTID_DISTINGUISH_SERVER_C2G || protid == PROTID_RECORD_CHARID_C2G)
            {
                prot_dispatch(np, id, header, protdata);
            }
            // judge from server or not
            else if(id != serverid)
            {
                ontranspond_sendprot_toserver(np, header, protdata, id);
            }
            // if the prot from senver then transpond to client
            else
            {
                int connid = prot_get_header_connid(protdata);
                log_debug("connid : %d\n", connid);
                ontranspond_sendprot_toclient(np, connid, header, protdata);
            }
        }
        free(protdata);
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
