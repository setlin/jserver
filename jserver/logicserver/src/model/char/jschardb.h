/*=============================================================================
#     FileName: jschardb.h
#         Desc: character database
#       Author: Setlin
#      Version: 0.0.1
#       Create: 22:00 2015/05/10
#   LastChange: 22:00 2015/05/10
#      History:
=============================================================================*/


#ifndef _JSCHARDB_H_
#define _JSCHARDB_H_


#include "../jsmemstd.h"

typedef struct _JSCHARINFO
{
    unsigned long charid;
    unsigned char charname[128];
    unsigned long gender;
    unsigned long age;
    unsigned char locality[256];
    unsigned char signature[1024];
    unsigned char grouplist[4096];
}JSCHARINFO, *PJSCHARINFO;


int JSCharDBInfoGet(PJSCHARINFO pjscharinfo);

int JSCharDBInfoClear(PJSCHARINFO pjscharinfo);



#endif














