#include "jsstd.h"
#include "log.h"
#include "buffer.h"
#include "net.h"
#include "handler.h"
#include "init.h"
#include "jsserver.h"
#include "jsdb.h"

void jsserver_net_server()
{
    struct net_pool* np = net_new();
    reghandler(np);
    init_prothandler(np);
    JSDBInit();

    // net pool, ip address, port
    net_connect(np, CONF_GATEWAY_IP, CONF_GATEWAY_PORT);

    for(;;)
    {
        net_loop(np, -1);
    }

    net_delete(&np);
    JSDBClose();
}




































