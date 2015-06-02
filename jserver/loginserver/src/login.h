/*=============================================================================
#     FileName: login.h
#         Desc: login model
#       Author: Setlin
#      Version: 0.0.1
#       Create: 21:03 2015/04/16
#   LastChange: 21:03 2015/04/16
#      History:
=============================================================================*/


#ifndef _LOGIN_H_
#define _LOGIN_H_

#include "prot.h"
#include "jsaccountinfo.h"

// login check
int login_check(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength);


#endif


