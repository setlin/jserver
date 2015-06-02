#include "jsstd.h"
#include "log.h"
#include "buffer.h"
#include "net.h"
#include "handler.h"
#include "init.h"
#include "jsgateway.h"

void jsgateway_net_server()
{
    struct net_pool* np = net_new();
    reghandler(np);
    init_prothandler(np);

    char* host = (char*)malloc(16);
    sprintf(host, "%s", "0.0.0.0");

    net_listen(np, host, 6300);

    for(;;)
    {
        net_loop(np, -1);
    }

    free(host);
    net_delete(&np);
}




































