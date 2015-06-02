/*=============================================================================
#     FileName: handler.h
#         Desc: handler of event
#       Author: Setlin
#      Version: 0.0.1
#       Create: 22:20 2015/03/28
#   LastChange: 22:20 2015/03/28
#      History:
=============================================================================*/
#ifndef _HANDLER_H_
#define _HANDLER_H_
#include "net.h"

void onaccept(struct net_pool* np, int listenfd, int clientfd);
void onconnected(struct net_pool* np, int id);
void onclose(struct net_pool* np, int id);
void onerror(struct net_pool* np, int id, int what);
void onrecv(struct net_pool* np, int id, int n);
void onsend(struct net_pool* np, int id, int n);

void reghandler(struct net_pool* np);

#endif
