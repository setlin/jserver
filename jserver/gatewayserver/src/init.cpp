#include "init.h"
#include "prot.h"


// initialize protocol handler
int init_prothandler(struct net_pool* np)
{
    log_debug("init_prothandler begin\n");
    // register protocol handler

    log_debug("protid %d\n", PROTID_DISTINGUISH_SERVER_C2G);
    NET_REG_PROTHANDLER(np, PROTID_DISTINGUISH_SERVER_C2G, prot_distinguish_prot100);

    log_debug("protid %d\n", PROTID_RECORD_CHARID_C2G);
    NET_REG_PROTHANDLER(np, PROTID_RECORD_CHARID_C2G, prot_savecharid);

    log_debug("init_prothandler finish\n");
}
