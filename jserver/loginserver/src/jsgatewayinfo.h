/*=============================================================================
#     FileName: jsgatewayinfo.h
#         Desc: gateway information
#       Author: Setlin
#      Version: 0.0.1
#       Create: 00:25 2015/04/22
#   LastChange: 00:25 2015/04/22
#      History:
=============================================================================*/

#ifndef _JSGATEWAYINFO_H_
#define _JSGATEWAYINFO_H_

#include "jsmemstd.h"

typedef struct _JSGATEWAYINFO{
    unsigned long gatewayid;
    unsigned char gatewayip[64];
    unsigned long gatewayport;
}JSGATEWAYINFO, *PJSGATEWAYINFO;

int JSGatewayinfoGet(PJSGATEWAYINFO pjsgatewayinfo);

#endif

