/*=============================================================================
#     FileName: jslogin.h
#         Desc: login logic
#       Author: Setlin
#      Version: 0.0.1
#       Create: 15:22 2015/04/07
#   LastChange: 15:22 2015/04/07
#      History:
=============================================================================*/

#ifndef _JSLOGIN_H_
#define _JSLOGIN_H_

#include "../../prot.h"

#define PROT_100_CLIENT_PROTDATA "MeW6Top5BAjUPwJyBAP2"

// check the prot 100 from client
int login_handler_answer_prot100_c2g(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength);

// check the prot 100 from client
int login_handler_answer_prot100_g2c(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength);

#endif

