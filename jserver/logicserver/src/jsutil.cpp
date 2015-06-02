/*=============================================================================
#     FileName: jsutil.cpp
#         Desc: util
#       Author: Setlin
#      Version: 0.0.1
#       Create: 17:06 2015/05/22
#   LastChange: 17:06 2015/05/22
#      History:
=============================================================================*/


#include "jsutil.h"


unsigned long jsutil_getcurtime()
{
    unsigned long curtime = 0;
    time_t t;
    t = time(0);
    curtime = t;

    return curtime;
}














