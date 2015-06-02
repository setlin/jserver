/*=============================================================================
#     FileName: jsgatewayinfo.cpp
#         Desc: gateway information
#       Author: Setlin
#      Version: 0.0.1
#       Create: 00:26 2015/04/22
#   LastChange: 00:26 2015/04/22
#      History:
=============================================================================*/


#include "jsgatewayinfo.h"

map<unsigned long, JSGATEWAYINFO> mapjsgatewayinfo;

static int JSGatewayinfoLoadData(char* szsql)
{
    int res = 0;

    MYSQL_RES* RES;
    RES = JSDBQuery(szsql);
    if(!RES)
    {
        return -1;
    }

    // 得到结果集列数
    int fieldnum = mysql_num_fields(RES);

    // 得到结果集列数据
    MYSQL_FIELD* fields = mysql_fetch_fields(RES);

    // 结果集行
    MYSQL_ROW row;

    // 循环处理行数据
    int rowno = 0;
    while((row = mysql_fetch_row(RES)))
    {
        // 处理每一行的每一列
        int i = 0;
        // 将结果保存至内存中

        JSGATEWAYINFO tempjsgatewayinfo;
        tempjsgatewayinfo.gatewayid = atoi(row[0]);
        memcpy(tempjsgatewayinfo.gatewayip, row[1], strlen(row[1]));
        tempjsgatewayinfo.gatewayport = atoi(row[2]);

        mapjsgatewayinfo.insert(pair<unsigned long, JSGATEWAYINFO>(tempjsgatewayinfo.gatewayid, tempjsgatewayinfo));
    }

    // 释放结果集内存
    mysql_free_result(RES);

    return res;
}


int JSGatewayinfoGet(PJSGATEWAYINFO pjsgatewayinfo)
{
    int res = 0;

    unsigned long gatewayid = pjsgatewayinfo->gatewayid;
    map<unsigned long, JSGATEWAYINFO>::iterator it = mapjsgatewayinfo.find(gatewayid);

    if(it != mapjsgatewayinfo.end())
    {
        JSGATEWAYINFO tempjsgatewayinfo;
        tempjsgatewayinfo = it->second;
        memcpy(pjsgatewayinfo, &tempjsgatewayinfo, sizeof(JSGATEWAYINFO));
    }
    else
    {
        char szsqlformat[JSDB_MAXSIZE_SQL];
        memset(szsqlformat, '\0', sizeof(szsqlformat));

        char szsql[JSDB_MAXSIZE_SQL];
        memset(szsql, '\0', sizeof(szsql));

        sprintf(szsqlformat, "%s", "SELECT gatewayid,gatewayip,gatewayport from jsgatewayinfo where gatewayid = %ld");
        sprintf(szsql, szsqlformat, gatewayid);

        res = JSGatewayinfoLoadData(szsql);

        if(res != -1)
        {
            JSGatewayinfoGet(pjsgatewayinfo);
        }

    }

    return res;
}


