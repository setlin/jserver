#include "jsstd.h"
#include "log.h"
#include "buffer.h"
#include "net.h"
#include "handler.h"
#include "init.h"
#include "jsdb.h"

void jstest_buffer()
{
    int fd = open("tmp.txt", O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    struct wbuffer* wb = wbuffer_new();
    wbuffer_setheader(wb, 2, 5);
    wbuffer_pushback(wb, "World", 5);
    wbuffer_setheader(wb, 2, 5);
    wbuffer_pushback(wb, "World", 5);

    debug_wbuffer_dump(wb, "wbuffer");
    debug_wbuffer_print(wb, "wbuffer");

    wbuffer_write(wb, fd, 14);

    wbuffer_delete(&wb);
    close(fd);


    fd = open("tmp.txt", O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    struct rbuffer* rb = rbuffer_new();
    int n = rbuffer_read(rb, fd, 7);
    log_debug("n = %d, fd = %d\n", n, fd);

    debug_rbuffer_dump(rb, "rbuffer");
    debug_rbuffer_print(rb, "rbuffer");

    int header = 0;
    header = rbuffer_getheader(rb, 2);
    log_debug("header = %d\n", header);

    char data[32];
    memset(data, '\0',32);
    rbuffer_popfront(rb, data, header);
    log_debug("data = %s\n", data);


    n = rbuffer_read(rb, fd, 7);
    log_debug("n = %d, fd = %d\n", n, fd);
    debug_rbuffer_dump(rb, "rbuffer");
    debug_rbuffer_print(rb, "rbuffer");

    header = rbuffer_getheader(rb, 2);
    log_debug("header = %d\n", header);

    memset(data, '\0',32);
    rbuffer_popfront(rb, data, header);
    log_debug("data = %s\n", data);

    rbuffer_delete(&rb);

    close(fd);
}

void jstest_net_server()
{
    struct net_pool* np = net_new();
    reghandler(np);
    init_prothandler(np);
    JSDBInit();

    char* host = (char*)malloc(16);
    sprintf(host, "%s", "0.0.0.0");

    net_listen(np, host, 6301);

    for(;;)
    {
        net_loop(np, -1);
    }

    free(host);
    net_delete(&np);
    JSDBClose();
}




































