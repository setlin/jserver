/*=============================================================================
#     FileName: jsfriend.h
#         Desc: friend system
#       Author: Setlin
#      Version: 0.0.1
#       Create: 17:05 2015/05/10
#   LastChange: 17:05 2015/05/10
#      History:
=============================================================================*/

#ifndef _JSFRIEND_H_
#define _JSFRIEND_H_

#include "../../prot.h"



// search friend
int friend_handler_searchfriend(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength);

// add friend
int friend_handler_addfriend(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength);

// delete friend
int friend_handler_deletefriend(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength);

// load friend
int friend_handler_loadfriend(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength);

#endif















