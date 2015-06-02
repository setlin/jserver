/*=============================================================================
#     FileName: jschar.h
#         Desc: character
#       Author: Setlin
#      Version: 0.0.1
#       Create: 21:49 2015/05/10
#   LastChange: 21:49 2015/05/10
#      History:
=============================================================================*/

#ifndef _JSCHAR_H_
#define _JSCHAR_H_


#include "jschardb.h"
#include "../../prot.h"


// get character information
int char_handler_getcharinfo_c2s(struct net_pool* np, int id, int seqid, int charid, uint8_t* data, int datalength);

// get charname by charid
int char_getcharnamebycharid(unsigned long charid, uint8_t* charname);

// get charinfo by charid
int char_getcharinfobycharid(PJSCHARINFO pjscharinfo);


#endif












