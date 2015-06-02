/*=============================================================================
#     FileName: jschat.h
#         Desc: chat model
#       Author: Setlin
#      Version: 0.0.1
#       Create: 10:43 2015/03/30
#   LastChange: 10:45 2015/03/30
#      History:
=============================================================================*/
#ifndef _JSCHAT_H_
#define _JSCHAT_H_

#include "../../prot.h"

// personal channel's handler
int chat_handler_channel_person_c2s(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength);

#endif
