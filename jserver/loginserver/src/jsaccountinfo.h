/*=============================================================================
#     FileName: jsaccountinfo.h
#         Desc: db table jsaccountinfo
#       Author: Setlin
#      Version: 0.0.1
#       Create: 21:05 2015/04/13
#   LastChange: 21:05 2015/04/13
#      History:
=============================================================================*/


#ifndef _JSACCOUNTINFO_H_
#define _JSACCOUNTINFO_H_

#include "jsmemstd.h"

typedef struct _JSACCOUNTINFO
{
    unsigned long charid;
    unsigned char passwd[128];
    unsigned long isValid;
}JSACCOUNTINFO, *PJSACCOUNTINFO;

int JSAccountInfoGet(PJSACCOUNTINFO pjsaccountinfo);

#endif



